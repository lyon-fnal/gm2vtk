////////////////////////////////////////////////////////////////////////
// Class:       Cone
// Plugin Type: producer (art v2_02_01)
// File:        Cone_module.cc
//
// Generated at Thu Aug 11 01:29:04 2016 by me using cetskelgen
// from cetlib version v1_19_00.
//
// A PRODUCER to put a MultiBlock data set representing a cone into the event
////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <iostream>

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"

#include "vtkSmartPointer.h"
#include "vtkConeSource.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkInformation.h"

#include "gm2viz/dumbEventDisplayNew/VtkVizData.hh"

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussQ.h"

namespace gm2viz {
  class Cone;
}


class gm2viz::Cone : public art::EDProducer {
public:
  explicit Cone(fhicl::ParameterSet const & p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  Cone(Cone const &) = delete;
  Cone(Cone &&) = delete;
  Cone & operator = (Cone const &) = delete;
  Cone & operator = (Cone &&) = delete;

  // Required functions.
  void produce(art::Event & e) override;

private:

  // Declare member data here.
  vtkSmartPointer<vtkPolyData> makeConeGrid();

  std::string myName_;
  unsigned int maxNum_;

  CLHEP::HepRandomEngine& engine_;  // this can be a reference because it is created by createEngine, which returns a ref
  CLHEP::RandFlat flatGen_;  // this cannot be a reference, because we create it with its c'tor
};


gm2viz::Cone::Cone(fhicl::ParameterSet const & p) :
  myName_("SHOULD_NEVER_SEE_THIS"),
  maxNum_(p.get<unsigned int>("maxNum", 10)),
  engine_( createEngine(get_seed_value(p))),
  flatGen_( engine_ )
{

  // Declare that we're making a VtkVizData
  produces< gm2viz::VtkVizData >();

  // Make the name to use for the VtkVizData
  std::ostringstream nameStream;
  nameStream << p.get<std::string>("module_type") << " (labelled " << p.get<std::string>("module_label") << ")";
  myName_ = nameStream.str();
}

void gm2viz::Cone::produce(art::Event & e)
{

  // Create the multiblockdataset
  vtkSmartPointer<vtkMultiBlockDataSet> mb = vtkSmartPointer<vtkMultiBlockDataSet>::New();

  // How many cones to make?
  unsigned int nCones = flatGen_.fireInt(maxNum_) + 1;
  mb->SetNumberOfBlocks(nCones);

  for ( unsigned int i=0; i < nCones; i++ ) {

    // Make the cone's grid
    auto grid = makeConeGrid();

    // Put it in the MultiBlock dataset
    std::ostringstream mbNameS;
    mbNameS << "Test Cone " << i+1;
    mb->SetBlock(i, grid);
    mb->GetMetaData(i)->Set(vtkCompositeDataSet::NAME(), mbNameS.str().c_str() );
  }

  // Put it into the event
  std::unique_ptr<gm2viz::VtkVizData> vizVtkData = std::make_unique<gm2viz::VtkVizData>(mb, myName_);
  e.put( std::move(vizVtkData) );
}

/// Create a cone and return its vtkPolyData grid
/// \return The vtkPolyData grid representing the cone
vtkSmartPointer<vtkPolyData> gm2viz::Cone::makeConeGrid() {

  // Make the cone
  auto cone = vtkSmartPointer<vtkConeSource>::New();
  cone->SetCenter( flatGen_.fire()*500.0, flatGen_.fire()*500.0, flatGen_.fire()*500.0 );
  cone->SetHeight( flatGen_.fire()*100.0 );
  cone->SetRadius( flatGen_.fire()*100.0 );
  cone->SetResolution(50);
  cone->Update();

  // Make the polydata
  vtkSmartPointer<vtkPolyData> grid = cone->GetOutput();
  return grid;
}


DEFINE_ART_MODULE(gm2viz::Cone)
