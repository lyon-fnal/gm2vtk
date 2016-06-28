// Implementation of CatalystAdapter_service

#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "fhiclcpp/ParameterSet.h"

#include "vtkCPDataDescription.h"
#include "vtkCPInputDataDescription.h"
#include "vtkCPProcessor.h"
#include "vtkCPPythonScriptPipeline.h"
#include "vtkNew.h"

#include "vtkIntArray.h"
#include "vtkTable.h"
#include "vtkSmartPointer.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <unistd.h> // for sleep

#include "gm2viz/adapter/CatalystAdapter_service.hh"

#include "cetlib/exception.h"

namespace gm2viz {

CatalystAdapter::CatalystAdapter(fhicl::ParameterSet const &p,
                                 art::ActivityRegistry &iRegistry)
    : vtkDataObjects_(), catalystProcessor_(vtkCPProcessor::New()),
      sleepusec_(p.get<int>("sleepusec", 100000)), registrationOpen_(true),
      vizCounter_(0), visualizeIt_(false) {
  // See
  // https://cdcvs.fnal.gov/redmine/projects/art/wiki/Guide_to_writing_and_using_services
  iRegistry.sPostBeginJob.watch(&CatalystAdapter::postBeginJob, *this);
  iRegistry.sPostProcessEvent.watch(&CatalystAdapter::postProcessEvent, *this);
  iRegistry.sPostEndJob.watch(&CatalystAdapter::postEndJob, *this);

  // Register the "eventInfo" object
  registerVTKData("eventInfo");
}

CatalystAdapter::~CatalystAdapter() {}

// Register name of dataset to declare intention to pass to Catalyst
void CatalystAdapter::registerVTKData(const std::string &name) {
  if (!registrationOpen_) {
    throw cet::exception("CATALYSTADAPTER") << "Registration is closed";
  }

  // Do we already have this string?
  if (vtkDataObjects_.find(name) == vtkDataObjects_.end()) {
    // Add it
    vtkDataObjects_[name] = nullptr;
  } else {
    throw cet::exception("CATALYSTADAPTER")
        << "Registration of VTKData with name " << name
        << " failed because name is already used";
  }
}

void CatalystAdapter::giveVTKData(const std::string &name,
                                  vtkSmartPointer<vtkDataObject> vtkData) {
  // Do we have this string?
  if (vtkDataObjects_.find(name) != vtkDataObjects_.end()) {
    // We have it
    vtkDataObjects_[name] = vtkData;

    // There will be something to visualize for this event!
    visualizeIt_ = true;
  } else {
    throw cet::exception("CATALYSTADAPTER")
        << "Giving VTKdata with unknown name " << name;
  }
}

void CatalystAdapter::postBeginJob() {
  closeRegistration();
  writePythonPipelineFile();
}

void CatalystAdapter::postProcessEvent(art::Event const &event) {
  if (visualizeIt_) {
    makeEventInfo(event);
  }
  coProcess(event.event());
}

void CatalystAdapter::postEndJob() { finalize(); }

void CatalystAdapter::closeRegistration() { registrationOpen_ = false; }

void CatalystAdapter::writePythonPipelineFile() {

  if (vtkDataObjects_.size() == 0) {
    LOG_INFO("CATALYSTADAPTER") << "No visualization data";
    return;
  }

  // Make the python pipeline file from the template

  // Form the data product creation lines
  std::stringstream createProducerLines;
  std::stringstream renameLines;
  std::stringstream freqsLine;
  freqsLine << "freqs = { ";

  for (auto anElement : vtkDataObjects_) {
    std::string name = anElement.first;
    std::cout << "NAME IS " << name << std::endl;
    createProducerLines << name
                        << " = coprocessor.CreateProducer(datadescription, '"
                        << name << "');";
    renameLines << "RenameSource('" << name << "', " << name << ");";
    freqsLine << "'" << name << "' : [], ";
  }
  freqsLine << "}";

  // Update the template
  std::stringstream sedCommand;
  sedCommand << "sed -e \"s/LLLL/" << createProducerLines.str()
             << "/\" -e \"s/MMMM/" << renameLines.str() << "/\" -e \"s/NNNN/"
             << freqsLine.str()
             << "/\" pythonPipeline.py_template > pythonPipeline.py";
  std::cout << "SED COMMAND" << sedCommand.str() << std::endl;

  std::system(sedCommand.str().c_str());

  // Fire up the Coprocessor
  catalystProcessor_->Initialize();
  vtkNew<vtkCPPythonScriptPipeline> pipeline;
  pipeline->Initialize("pythonPipeline.py");
  catalystProcessor_->AddPipeline(pipeline.GetPointer());
}

// Make the event information block
void CatalystAdapter::makeEventInfo(const art::Event &e) {
  vtkSmartPointer<vtkTable> t = vtkSmartPointer<vtkTable>::New();

  vtkSmartPointer<vtkIntArray> eventNum = vtkSmartPointer<vtkIntArray>::New();
  eventNum->SetName("eventNum");
  eventNum->InsertNextValue(e.event());

  vtkSmartPointer<vtkIntArray> vizCounter = vtkSmartPointer<vtkIntArray>::New();
  vizCounter->SetName("vizCounter");
  vizCounter->InsertNextValue(vizCounter_);

  vtkSmartPointer<vtkIntArray> subRunNum = vtkSmartPointer<vtkIntArray>::New();
  subRunNum->SetName("subRunNum");
  subRunNum->InsertNextValue(e.subRun());

  vtkSmartPointer<vtkIntArray> runNum = vtkSmartPointer<vtkIntArray>::New();
  runNum->SetName("runNum");
  runNum->InsertNextValue(e.run());

  // Fill the table
  t->AddColumn(eventNum);
  t->AddColumn(vizCounter);
  t->AddColumn(subRunNum);
  t->AddColumn(runNum);

  // Add it
  giveVTKData("eventInfo", t);
}

// Visualize!
void CatalystAdapter::coProcess(int eventNum) {

  if (vtkDataObjects_.size() == 0) {
    return;
  }

  if (!visualizeIt_) {
    // No visualization data were filled for this event
    return;
  }

  vtkNew<vtkCPDataDescription> dataDescription;

  for (auto elem : vtkDataObjects_) {
    dataDescription->AddInput(elem.first.c_str());
    // std::cout << elem.first << std::endl;
    // elem.second->Print( std::cout );
  }

  std::cout << "Event " << eventNum << "/ Time step " << vizCounter_
            << std::endl;
  dataDescription->SetTimeData(eventNum, vizCounter_);
  vizCounter_++;

  // Are we going to send this event to the pipeline?
  if (catalystProcessor_->RequestDataDescription(
          dataDescription.GetPointer()) != 0) {

    // Load the data
    for (auto elem : vtkDataObjects_) {
      if (!elem.second) {
        throw cet::exception("CATALYSTADAPTER") << "VTK data for " << elem.first
                                                << " was not filled";
      }
      dataDescription->GetInputDescriptionByName(elem.first.c_str())
          ->SetGrid(elem.second);
    }

    // GO!
    catalystProcessor_->CoProcess(dataDescription.GetPointer()); // Go!

    // Now pause
    // std::cout << "Event #" << e.event() << "  Pausing... " << std::endl;
    // std::cin.ignore();
    usleep(sleepusec_);

    // Reset the visualizeIt flag
    visualizeIt_ = false;
  }

  // We're done with this event. Destroy the objects
  // dataDescription->Delete();

  // Delete the VTK data
  // for ( auto elem : vtkDataObjects_ ) {
  //  elem.second->Delete();
  //}
}

void CatalystAdapter::finalize() {
  catalystProcessor_->Finalize();
  catalystProcessor_->Delete();
}

} // Namespace

using gm2viz::CatalystAdapter;
DEFINE_ART_SERVICE(CatalystAdapter)
