////////////////////////////////////////////////////////////////////////
// Class:       DumbVTK
// Module Type: analyzer
// File:        DumbVTK_module.cc
//
// Generated at Tue Oct 27 23:04:39 2015 by Adam Lyon using artmod
// from cetpkgsupport v1_08_04.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "gm2catalyst/adapter/CatalystAdapter_service.hh"

#include "gm2ringsim/calo/CaloArtRecord.hh"

#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkConeSource.h"

namespace gm2catalyst {
  class DumbVTK;
}

const std::string vtkDataName = "dumb";

class gm2catalyst::DumbVTK : public art::EDAnalyzer {
public:
  explicit DumbVTK(fhicl::ParameterSet const & p);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  DumbVTK(DumbVTK const &) = delete;
  DumbVTK(DumbVTK &&) = delete;
  DumbVTK & operator = (DumbVTK const &) = delete;
  DumbVTK & operator = (DumbVTK &&) = delete;

  // Required functions.
  void beginJob() override;
  void analyze(art::Event const & e) override;


private:

  std::string caloArtRecordInputTag_;
  
};


gm2catalyst::DumbVTK::DumbVTK(fhicl::ParameterSet const & p)
  :
  EDAnalyzer(p)
{}

void gm2catalyst::DumbVTK::beginJob() {
  // Get the CatalystAdapter service and register
  art::ServiceHandle<CatalystAdapter> adapter;
  adapter->registerVTKData(vtkDataName);
}

void gm2catalyst::DumbVTK::analyze(art::Event const & e)
{
  // Build the VTK structures
  vtkConeSource* cone = vtkConeSource::New();
  cone->SetHeight( 20 );
  cone->SetRadius( 30 );
  cone->SetResolution(50);
  cone->Update();
  vtkSmartPointer<vtkPolyData> grid = cone->GetOutput();
  
  // Give this to the adapter service
  art::ServiceHandle<CatalystAdapter> adapter;
  adapter->giveVTKData(vtkDataName, grid);
}

DEFINE_ART_MODULE(gm2catalyst::DumbVTK)