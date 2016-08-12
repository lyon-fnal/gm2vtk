////////////////////////////////////////////////////////////////////////
// Class:       LiveVizFilter
// Plugin Type: filter (art v2_02_01)
// File:        LiveVizFilter_module.cc
//
// Generated at Thu Aug 11 23:24:23 2016 by me using cetskelgen
// from cetlib version v1_19_00.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <memory>

#include "gm2viz/dumbEventDisplayNew/CatalystLive.h"

namespace gm2viz {
  class LiveVizFilter;
}


class gm2viz::LiveVizFilter : public art::EDFilter {
public:
  explicit LiveVizFilter(fhicl::ParameterSet const & p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  LiveVizFilter(LiveVizFilter const &) = delete;
  LiveVizFilter(LiveVizFilter &&) = delete;
  LiveVizFilter & operator = (LiveVizFilter const &) = delete;
  LiveVizFilter & operator = (LiveVizFilter &&) = delete;

  // Required functions.
  bool filter(art::Event & e) override;

private:

  // Declare member data here.

};


gm2viz::LiveVizFilter::LiveVizFilter(fhicl::ParameterSet const & p)
// :
// Initialize member data here.
{
  // Call appropriate produces<>() functions here.
}

bool gm2viz::LiveVizFilter::filter(art::Event & e)
{
  // Get the CatalystLive service
  art::ServiceHandle<gm2viz::CatalystLive> catalyst;
  return catalyst->doWeVizQuestionMark();
}

DEFINE_ART_MODULE(gm2viz::LiveVizFilter)
