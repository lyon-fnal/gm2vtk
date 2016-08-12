////////////////////////////////////////////////////////////////////////
// Class:       CatalystLiveVtkViz
// Plugin Type: analyzer (art v2_02_01)
// File:        CatalystLiveVtkViz_module.cc
//
// Generated at Thu Aug 11 21:54:08 2016 by me using cetskelgen
// from cetlib version v1_19_00.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "gm2viz/dumbEventDisplayNew/CatalystLive.h"

#include "vtkMultiBlockDataSet.h"
#include "vtkSmartPointer.h"

#include "gm2viz/dumbEventDisplayNew/VtkVizData.hh"

#include "gm2viz/dumbEventDisplayNew/CatalystLive.h"

#include <unistd.h> // for sleep

namespace gm2viz {
  class CatalystLiveVtkViz;
}


class gm2viz::CatalystLiveVtkViz : public art::EDAnalyzer {
public:
  explicit CatalystLiveVtkViz(fhicl::ParameterSet const & p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  CatalystLiveVtkViz(CatalystLiveVtkViz const &) = delete;
  CatalystLiveVtkViz(CatalystLiveVtkViz &&) = delete;
  CatalystLiveVtkViz & operator = (CatalystLiveVtkViz const &) = delete;
  CatalystLiveVtkViz & operator = (CatalystLiveVtkViz &&) = delete;

  // Required functions.
  void analyze(art::Event const & e) override;

private:

  // Declare member data here.
  int sleepusec_;
};


gm2viz::CatalystLiveVtkViz::CatalystLiveVtkViz(fhicl::ParameterSet const & p)
  :
  EDAnalyzer(p),
  sleepusec_(p.get<int>("sleepusec", 100000))
 // More initializers here.
{}

void gm2viz::CatalystLiveVtkViz::analyze(art::Event const & e)
{
  // Make the multiblock dataset to fill
  vtkSmartPointer<vtkMultiBlockDataSet> mb = vtkSmartPointer<vtkMultiBlockDataSet>::New();

  // Get the vtk objects in the event
  std::vector<art::Handle<gm2viz::VtkVizData> > vizes;
  e.getManyByType(vizes);

  // We know how many blocks we'll have, so fill it
  mb->SetNumberOfBlocks( vizes.size() );

  unsigned blockno = 0;
  for ( auto& aViz : vizes) {
    aViz->insertIntoMultiBlock(mb, blockno);
    blockno++;
  }

  // Get the CatalystLive service
  art::ServiceHandle<gm2viz::CatalystLive> catalyst;
  catalyst->addVizesData(mb.GetPointer());

  LOG_INFO("CatalystLiveVtkViz") << "Visualizing!!";

  catalyst->vizit();
  usleep(sleepusec_);
}

DEFINE_ART_MODULE(gm2viz::CatalystLiveVtkViz)
