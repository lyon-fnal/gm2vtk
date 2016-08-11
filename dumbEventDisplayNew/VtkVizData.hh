//
// Created by Adam Lyon on 8/11/16.
//

#ifndef V7VIZCLION_VTKVIZDATA_H
#define V7VIZCLION_VTKVIZDATA_H

#include <string>

#include "vtkSmartPointer.h"
#include "vtkMultiBlockDataSet.h"

namespace gm2viz {
  class VtkVizData;
}

/// This is a transient object to hold Vtk visualization data (generally a VTKMultiBlockDataSet and a name
class gm2viz::VtkVizData {
public:
  VtkVizData(vtkSmartPointer<vtkMultiBlockDataSet> mb, const std::string name) :
      mb_(mb),
      name_(name)
  {}



#ifndef __ROOTCLING__

  /// Put my multiBlockDataSet into a perhaps bigger MultiblockDataSet at entry blockno
  /// \param mbToFill The multiBlockDataSet to insert into
  /// \param blockno The block number
  void insertIntoMultiBlock(vtkSmartPointer<vtkMultiBlockDataSet> mbToFill, unsigned int blockno);

  /// Get the name of this VtkVizData
  /// \return name of this VtkVizData (not necessarily the name of the multiblock dataset)
  const std::string& name() { return name_; }

#endif

private:
  vtkSmartPointer<vtkMultiBlockDataSet> mb_;
  std::string name_;

};

#endif