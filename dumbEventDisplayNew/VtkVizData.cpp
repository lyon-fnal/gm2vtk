//
// Created by Adam Lyon on 8/11/16.
//

#include "VtkVizData.hh"
#include "vtkInformation.h"

void gm2viz::VtkVizData::insertIntoMultiBlock(vtkSmartPointer<vtkMultiBlockDataSet> mbToFill, unsigned int blockno) const {
    mbToFill->SetBlock(blockno, mb_);
    mbToFill->GetMetaData(blockno)->Set( vtkCompositeDataSet::NAME(), name_.c_str());
}
