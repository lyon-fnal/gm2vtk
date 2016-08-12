////////////////////////////////////////////////////////////////////////
// Class:       CatalystLive
// Plugin Type: service (art v2_02_01)
// File:        CatalystLive_service.cc
//
// Generated at Thu Aug 11 21:55:47 2016 by me using cetskelgen
// from cetlib version v1_19_00.
////////////////////////////////////////////////////////////////////////

#include "gm2viz/dumbEventDisplayNew/CatalystLive.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "fhiclcpp/ParameterSet.h"

#include "vtkCPDataDescription.h"
#include "vtkCPInputDataDescription.h"
#include "vtkCPProcessor.h"
#include "vtkCPPythonScriptPipeline.h"
#include "vtkTable.h"
#include "vtkIntArray.h"
#include "vtkDataObject.h"
#include "vtkSmartPointer.h"

#include <memory>
#include <gm2viz/adapter/CatalystAdapter_service.hh>

gm2viz::CatalystLive::CatalystLive(fhicl::ParameterSet const & p, art::ActivityRegistry & areg) :
   catalystProcessor_( vtkCPProcessor::New() ),
   dataDescription_(nullptr),
   eventInfo_(nullptr),
   pythonPipelineFileName_( p.get<std::string>("pythonPipelineFileName", "pythonPipeline.py")),
   vizCounter_(0)
{
  areg.sPostBeginJob.watch(&CatalystLive::postBeginJob, *this);
  areg.sPostEndJob.watch(&CatalystLive::postEndJob, *this);
  areg.sPreProcessEvent.watch(&CatalystLive::preProcessEvent, *this);
  areg.sPostProcessEvent.watch(&CatalystLive::postProcessEvent, *this);
}

void gm2viz::CatalystLive::postEndJob() {
  catalystProcessor_->Finalize();
  catalystProcessor_->Delete();
}

void gm2viz::CatalystLive::postBeginJob() {
  // Initialize the processor
  catalystProcessor_->Initialize();
  vtkCPPythonScriptPipeline* pipeline = vtkCPPythonScriptPipeline::New();
  pipeline->Initialize(pythonPipelineFileName_.c_str());
  catalystProcessor_->AddPipeline(pipeline);
}

bool gm2viz::CatalystLive::doWeVizQuestionMark() {
  return catalystProcessor_->RequestDataDescription( dataDescription_ ) != 0;
}

void gm2viz::CatalystLive::preProcessEvent(art::Event const &e) {

  // Make the event info table
  makeEventInfo(e);

  // Add it to the data
  dataDescription_ = vtkCPDataDescription::New();
  dataDescription_->AddInput("eventInfo");
  dataDescription_->AddInput("vizes");

  dataDescription_->SetTimeData(e.event(), vizCounter_);
}

void gm2viz::CatalystLive::makeEventInfo(art::Event const &e) {
  eventInfo_ = vtkTable::New();

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
  eventInfo_->AddColumn(runNum);
  eventInfo_->AddColumn(subRunNum);
  eventInfo_->AddColumn(eventNum);
  eventInfo_->AddColumn(vizCounter);
}

void gm2viz::CatalystLive::postProcessEvent(art::Event const &e) {
  vizCounter_++;
  eventInfo_->Delete();
  dataDescription_->Delete();
}

void gm2viz::CatalystLive::addVizesData( vtkDataObject* vtkData ) {
  dataDescription_->GetInputDescriptionByName("vizes")->SetGrid( vtkData );
}

void gm2viz::CatalystLive::vizit() {
  // Add the event information table
  dataDescription_->GetInputDescriptionByName("eventInfo")->SetGrid( eventInfo_ );

  // Go!
  catalystProcessor_->CoProcess(dataDescription_);
}


DEFINE_ART_SERVICE(gm2viz::CatalystLive)
