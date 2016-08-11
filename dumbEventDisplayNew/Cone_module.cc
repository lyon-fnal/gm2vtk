////////////////////////////////////////////////////////////////////////
// Class:       Cone
// Plugin Type: producer (art v2_02_01)
// File:        Cone_module.cc
//
// Generated at Thu Aug 11 01:29:04 2016 by me using cetskelgen
// from cetlib version v1_19_00.
//
// Make a VtkMultiblock dataset and put a cube in it.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"

#include "vtkSmartPointer.h"
#include "vtkConeSource.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkInformation.h"

#include "gm2viz/dumbEventDisplayNew/VtkVizData.hh"

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
  vtkSmartPointer<vtkPolyData> makeConeGrid() const;
  vtkSmartPointer<vtkMultiBlockDataSet> makeMultiBlockForConeGrid(vtkSmartPointer<vtkPolyData> grid) const;
};


gm2viz::Cone::Cone(fhicl::ParameterSet const & p)
// :
// Initialize member data here.
{
  produces< gm2viz::VtkVizData >();
}

void gm2viz::Cone::produce(art::Event & e)
{

  // TODO: Add a way to check if a ParaView client is connected (needs to be consistent for all producers/analysers for this event)

  // Make the cone's grid
  auto grid = makeConeGrid();

  // Make a multiblock dataset out of the cone grid
  auto mb = makeMultiBlockForConeGrid(grid);

  // Put it into the event
  std::unique_ptr<gm2viz::VtkVizData> vizVtkData = std::make_unique<gm2viz::VtkVizData>(mb, "fromConeModule");
  e.put( std::move(vizVtkData) );
}

/// Create a cone and return its vtkPolyData grid
/// \return The vtkPolyData grid representing the cone
vtkSmartPointer<vtkPolyData> gm2viz::Cone::makeConeGrid() const {

  // Make the cone
  auto cone = vtkSmartPointer<vtkConeSource>::New();
  cone->SetHeight(30);
  cone->SetRadius(20);
  cone->SetResolution(50);
  cone->Update();

  // Make the polydata
  vtkSmartPointer<vtkPolyData> grid = cone->GetOutput();
  return grid;
}

/// Generate a MultiBlockDataSet from the cone's grid
/// \param grid The cone's grid (vtkPolyData)
/// \return The MultoBlockDataSet
vtkSmartPointer<vtkMultiBlockDataSet> gm2viz::Cone::makeMultiBlockForConeGrid(vtkSmartPointer<vtkPolyData> grid) const {

  // Make the multiblock dataset
  vtkSmartPointer<vtkMultiBlockDataSet> mb = vtkSmartPointer<vtkMultiBlockDataSet>::New();
  mb->SetNumberOfBlocks(1);
  const unsigned int blockno = 0; // Need to do this so that GetMetaData below doesn't confuse 0 with a pointer
  mb->SetBlock(blockno, grid);
  mb->GetMetaData(blockno)->Set(vtkCompositeDataSet::NAME(), "Test cone");
  return mb;
}


DEFINE_ART_MODULE(gm2viz::Cone)
