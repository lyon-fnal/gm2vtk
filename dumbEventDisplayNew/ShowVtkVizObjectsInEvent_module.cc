////////////////////////////////////////////////////////////////////////
// Class:       ShowVtkVizObjectsInEvent
// Plugin Type: analyzer (art v2_02_01)
// File:        ShowVtkVizObjectsInEvent_module.cc
//
// Generated at Thu Aug 11 15:07:23 2016 by me using cetskelgen
// from cetlib version v1_19_00.
//
// Analyzer to show the names of the VtkVizData objects in the event
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"

#include "gm2viz/dumbEventDisplayNew/VtkVizData.hh"

namespace gm2viz {
  class ShowVtkVizObjectsInEvent;
}


class gm2viz::ShowVtkVizObjectsInEvent : public art::EDAnalyzer {
public:
  explicit ShowVtkVizObjectsInEvent(fhicl::ParameterSet const & p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  ShowVtkVizObjectsInEvent(ShowVtkVizObjectsInEvent const &) = delete;
  ShowVtkVizObjectsInEvent(ShowVtkVizObjectsInEvent &&) = delete;
  ShowVtkVizObjectsInEvent & operator = (ShowVtkVizObjectsInEvent const &) = delete;
  ShowVtkVizObjectsInEvent & operator = (ShowVtkVizObjectsInEvent &&) = delete;

  // Required functions.
  void analyze(art::Event const & e) override;

private:

  // Declare member data here.

};


gm2viz::ShowVtkVizObjectsInEvent::ShowVtkVizObjectsInEvent(fhicl::ParameterSet const & p)
  :
  EDAnalyzer(p)  // ,
 // More initializers here.
{}

void gm2viz::ShowVtkVizObjectsInEvent::analyze(art::Event const & e)
{
  // Let's pull all of the VtkVizData objects out of the event
  std::vector<art::Handle<gm2viz::VtkVizData> > vizes;
  e.getManyByType(vizes);

  // Let's see what we got!
  LOG_VERBATIM("ShowVtkVizObjectsInEvent") << "VtkVizData Objects in this event:";
  for ( auto& aViz : vizes ) {
    LOG_VERBATIM("ShowVtkVizObjectsInEvent") << "  Got VtkVizData object with name " << aViz->name();
  }

}

DEFINE_ART_MODULE(gm2viz::ShowVtkVizObjectsInEvent)
