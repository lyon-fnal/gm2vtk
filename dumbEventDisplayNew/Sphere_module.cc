////////////////////////////////////////////////////////////////////////
// Class:       Sphere
// Plugin Type: producer (art v2_02_01)
// File:        Sphere_module.cc
//
// Generated at Thu Aug 11 01:29:04 2016 by me using cetskelgen
// from cetlib version v1_19_00.
//
// A PRODUCER to put a MultiBlock data set representing a Sphere into the event
////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <iostream>

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"

#include "vtkSmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkInformation.h"

#include "gm2viz/dumbEventDisplayNew/VtkVizData.hh"

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussQ.h"

namespace gm2viz {
  class Sphere;
}


class gm2viz::Sphere : public art::EDProducer {
public:
  explicit Sphere(fhicl::ParameterSet const & p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  Sphere(Sphere const &) = delete;
  Sphere(Sphere &&) = delete;
  Sphere & operator = (Sphere const &) = delete;
  Sphere & operator = (Sphere &&) = delete;

  // Required functions.
  void produce(art::Event & e) override;

private:

  // Declare member data here.
  vtkSmartPointer<vtkPolyData> makeSphereGrid();
  vtkSmartPointer<vtkMultiBlockDataSet> makeMultiBlockForSphereGrid(vtkSmartPointer<vtkPolyData> grid) const;

  std::string myName_;

  CLHEP::HepRandomEngine& engine_;  // this can be a reference because it is created by createEngine, which returns a ref
  CLHEP::RandFlat flatGen_;  // this cannot be a reference, because we create it with its c'tor

};

gm2viz::Sphere::Sphere(fhicl::ParameterSet const & p) :
  myName_("SHOULD_NEVER_SEE_THIS"),
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

void gm2viz::Sphere::produce(art::Event & e)
{
  // Make the Sphere's grid
  auto grid = makeSphereGrid();

  // Make a multiblock dataset out of the Sphere grid
  auto mb = makeMultiBlockForSphereGrid(grid);

  // Put it into the event
  std::unique_ptr<gm2viz::VtkVizData> vizVtkData = std::make_unique<gm2viz::VtkVizData>(mb, myName_);
  e.put( std::move(vizVtkData) );
}

/// Create a Sphere and return its vtkPolyData grid
/// \return The vtkPolyData grid representing the Sphere
vtkSmartPointer<vtkPolyData> gm2viz::Sphere::makeSphereGrid()  {

  // Make the Sphere
  auto sphere = vtkSmartPointer<vtkSphereSource>::New();
  sphere->SetCenter( flatGen_.fire()*500.0, flatGen_.fire()*500.0, flatGen_.fire()*500.0 );
  sphere->SetRadius( flatGen_.fire()*100.0 );
  sphere->SetThetaResolution( flatGen_.fire()*100.0 );
  sphere->SetPhiResolution( flatGen_.fire()*100.0 );
  sphere->Update();

  // Make the polydata
  vtkSmartPointer<vtkPolyData> grid = sphere->GetOutput();
  return grid;
}

/// Generate a MultiBlockDataSet from the Sphere's grid
/// \param grid The Sphere's grid (vtkPolyData)
/// \return The MultoBlockDataSet
vtkSmartPointer<vtkMultiBlockDataSet> gm2viz::Sphere::makeMultiBlockForSphereGrid(vtkSmartPointer<vtkPolyData> grid) const {

  // Make the multiblock dataset
  vtkSmartPointer<vtkMultiBlockDataSet> mb = vtkSmartPointer<vtkMultiBlockDataSet>::New();
  mb->SetNumberOfBlocks(1);
  const unsigned int blockno = 0; // Need to do this so that GetMetaData below doesn't confuse 0 with a pointer
  mb->SetBlock(blockno, grid);
  mb->GetMetaData(blockno)->Set(vtkCompositeDataSet::NAME(), "Test Sphere");
  return mb;
}


DEFINE_ART_MODULE(gm2viz::Sphere)
