#ifndef CatalystLive_h
#define CatalystLive_h
////////////////////////////////////////////////////////////////////////
// Class:       CatalystLive
// Plugin Type: service (art v2_02_01)
// File:        CatalystLive.h
//
// Generated at Thu Aug 11 21:55:47 2016 by me using cetskelgen
// from cetlib version v1_19_00.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"

#include <string>

class vtkCPProcessor;
class vtkCPDataDescription;
class vtkTable;
class vtkDataObject;

namespace fhicl {
  class ParameterSet;
}

namespace art {
  class ActivityRegistry;
}

namespace gm2viz {
  class CatalystLive;
}


class gm2viz::CatalystLive {
public:
  explicit CatalystLive(fhicl::ParameterSet const & p, art::ActivityRegistry & areg);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  void postBeginJob();
  void postEndJob();
  void preProcessEvent(art::Event const &e);
  void postProcessEvent(art::Event const &e);

  bool doWeVizQuestionMark();
  void addVizesData(vtkDataObject* vtkData);
  void vizit();

private:

  vtkCPProcessor* catalystProcessor_;
  vtkCPDataDescription* dataDescription_;
  vtkTable* eventInfo_;
  std::string pythonPipelineFileName_;
  unsigned int vizCounter_;

  void makeEventInfo(art::Event const &e);

};

DECLARE_ART_SERVICE(gm2viz::CatalystLive, LEGACY)
#endif /* CatalystLive_h */
