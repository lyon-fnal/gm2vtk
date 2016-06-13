// CatalystAdapter_service -- handles the catalyst communication between art and
// ParaView (Catalyst)

// For now does stuff through the Python pipeline file. In future, maybe directly go through
// vtkLiveInsituLink (see http://www.paraview.org/ParaQ/Doc/Nightly/html/classvtkLiveInsituLink.html)
// and see http://www.paraview.org/ParaView/Doc/Nightly/www/py-doc/_modules/paraview/coprocessing.html#CoProcessor.DoLiveVisualization

// This is a service - modules can call this service to
//   a) During BeingJob, register their intention to supply VTK data to Catalyst by giving a name for the data
//   b) In an event, pass their VTK data (via vtkDataObject*) to the service with the registered name.

// This service does the following:
//   1) Accept new registrations of vtk data set names. This should happen before the end of the "begin job" sequence
//   2) At the end of "begin job", write the python pipeline file and load it (need to know dataset names here)
//   3) Accept and hold vtkDataObject* from modules to fill input data descriptions
//   4) At the end of processing an event, run catalyst on the passed in data. Make sure all are there. Delete when done
//   5) Perhaps sleep for a certain amount of time instead of pausing (pausing at input confuses ParaView)


#ifndef CATALYSTADAPTERSERVICE
#define CATALYSTADAPTERSERVICE

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Principal/Event.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "vtkSmartPointer.h"
#include "vtkDataObject.h"

#include <unordered_map>

class vtkDataObject;
class vtkCPProcessor;

namespace gm2catalyst {

  class CatalystAdapter {
    
  public:
    
    CatalystAdapter( fhicl::ParameterSet const&, art::ActivityRegistry& );
    virtual ~CatalystAdapter();
    
    void registerVTKData(const std::string& name);
    void giveVTKData(const std::string& name, vtkSmartPointer<vtkDataObject> vtkData);
    
    void postBeginJob();
    void postProcessEvent(art::Event const&);
    void postEndJob();
    
  private:

    void closeRegistration();
    void writePythonPipelineFile();
    
    void makeEventInfo(art::Event const&);
    void coProcess(int eventNum);
    
    void finalize();
    
    std::unordered_map<std::string, vtkSmartPointer<vtkDataObject> > vtkDataObjects_;
    vtkCPProcessor* catalystProcessor_;
    
    int sleepusec_;
    bool registrationOpen_;
    int vizCounter_;
    bool visualizeIt_;
    
  };
}

using gm2catalyst::CatalystAdapter;
DECLARE_ART_SERVICE(CatalystAdapter,LEGACY)


#endif
