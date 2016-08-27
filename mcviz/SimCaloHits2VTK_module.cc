////////////////////////////////////////////////////////////////////////
// Class:       SimCaloHits2VTK
// Plugin Type: producer (art v2_02_01)
// File:        SimCaloHits2VTK_module.cc
//
// Generated at Wed Aug 24 00:31:55 2016 by me using cetskelgen
// from cetlib version v1_19_00.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "artvtk/artvtk/VtkVizData.hh"

#include "gm2ringsim/calo/CaloArtRecord.hh"

#include "vtkSmartPointer.h"
#include "vtkFloatArray.h"
#include "vtkIntArray.h"
#include "vtkStringArray.h"
#include "vtkPointData.h"
#include "vtkCell.h"
#include "vtkPolyData.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkInformation.h"

#include <memory>

namespace gm2viz {
  class SimCaloHits2VTK;
}


class gm2viz::SimCaloHits2VTK : public art::EDProducer {
public:
  explicit SimCaloHits2VTK(fhicl::ParameterSet const & p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  SimCaloHits2VTK(SimCaloHits2VTK const &) = delete;
  SimCaloHits2VTK(SimCaloHits2VTK &&) = delete;
  SimCaloHits2VTK & operator = (SimCaloHits2VTK const &) = delete;
  SimCaloHits2VTK & operator = (SimCaloHits2VTK &&) = delete;

  // Required functions.
  void produce(art::Event & e) override;

private:

  // Declare member data here.
  std::string caloArtRecordInputTag_;
  std::string myName_;
};


gm2viz::SimCaloHits2VTK::SimCaloHits2VTK(fhicl::ParameterSet const & p) :
  caloArtRecordInputTag_( p.get<std::string>("caloArtRecordInputTag", "artg4:calorimeter") ),
  myName_("SHOULD NEVER SEE THIS")
{
  // Declare that we're making a VtkVizData
  produces< artvtk::VtkVizData >();

  // Make the name to use for the VtkVizData
  std::ostringstream nameStream;
  nameStream << p.get<std::string>("module_type") << " (labelled " << p.get<std::string>("module_label") << ")";
  myName_ = nameStream.str();
}

void gm2viz::SimCaloHits2VTK::produce(art::Event & e)
{
  // Initialize
  // Positions and cells
  auto pts    = vtkSmartPointer<vtkPoints>::New();
  auto verts  = vtkSmartPointer<vtkCellArray>::New();

  // Energy
  auto energy = vtkSmartPointer<vtkFloatArray>::New();
  energy->SetName("energy");

  // time
  auto hitTime = vtkSmartPointer<vtkFloatArray>::New();
  hitTime->SetName("hitTime");

  // Parent ID
  auto parentID = vtkSmartPointer<vtkIntArray>::New();
  parentID->SetName("parentID");

  // Track ID
  auto trackID = vtkSmartPointer<vtkIntArray>::New();
  trackID->SetName("trackID");

  // Calorimeter Number
  auto caloNum = vtkSmartPointer<vtkIntArray>::New();
  caloNum->SetName("caloNum");

  // turn
  auto turn = vtkSmartPointer<vtkIntArray>::New();
  turn->SetName("turn");

  // Particle name
  auto particleName = vtkSmartPointer<vtkStringArray>::New();
  particleName->SetName("particleName");

  // Momentum
  vtkSmartPointer<vtkFloatArray>  p = vtkSmartPointer<vtkFloatArray>::New();
  p->SetName("p");
  p->SetNumberOfComponents(3);  // each element is a 3-vector
  p->SetComponentName(0, "px");
  p->SetComponentName(1, "py");
  p->SetComponentName(2, "pz");

  // Get calorimeter hits
  auto caloVectorH = e.getValidHandle<gm2ringsim::CaloArtRecordCollection>( caloArtRecordInputTag_ );

  if ( caloVectorH.isValid() ) {
    auto caloVector = *caloVectorH;

    // Run all hits  set the points first
    for ( auto aHit : caloVector ) {

      // Set the position
      vtkIdType pid[1];  // Make a one element array
      pid[0] = pts->InsertNextPoint(aHit.x, aHit.y, aHit.z);  // Returns the position in the point array

      // Make the vertex cell
      verts->InsertNextCell(1, pid); // The 1 indicates a one element cell

      // Fill arrays
      energy->InsertNextValue( aHit.energy );
      hitTime->InsertNextValue( aHit.time );
      parentID->InsertNextValue( aHit.parent_ID );
      trackID->InsertNextValue( aHit.trackID );
      turn->InsertNextValue( aHit.turn );
      caloNum->InsertNextValue( aHit.caloNum );
      particleName->InsertNextValue( aHit.particle_name );
      p->InsertNextTuple3( aHit.px, aHit.py, aHit.pz );
    }
  }

  // Make the polydata
  auto grid = vtkSmartPointer<vtkPolyData>::New();
  grid->SetPoints(pts);
  grid->SetVerts(verts);
  grid->GetPointData()->AddArray(energy);
  grid->GetPointData()->AddArray(hitTime);
  grid->GetPointData()->AddArray(parentID);
  grid->GetPointData()->AddArray(trackID);
  grid->GetPointData()->AddArray(turn);
  grid->GetPointData()->AddArray(caloNum);
  grid->GetPointData()->AddArray(particleName);
  grid->GetPointData()->AddArray(p);

  // Put it in a Multiblock dataset
  auto mb = vtkSmartPointer<vtkMultiBlockDataSet>::New();
  mb->SetNumberOfBlocks(1);
  const unsigned int blockno = 0; // Need to do this so that GetMetaData below doesn't confuse 0 with a pointer
  mb->SetBlock(blockno, grid);
  mb->GetMetaData(blockno)->Set(vtkCompositeDataSet::NAME(), "SimCaloHits");

  // Put it in the event
  auto vizVtkData = std::make_unique<artvtk::VtkVizData>(mb, myName_);
  e.put ( std::move (vizVtkData) );
}


DEFINE_ART_MODULE(gm2viz::SimCaloHits2VTK)
