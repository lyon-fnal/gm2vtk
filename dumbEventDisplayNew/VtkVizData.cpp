//
// Created by Adam Lyon on 8/11/16.
//

#include "gm2viz/dumbEventDisplayNew/VtkVizData.hh"
#include "vtkInformation.h"

void gm2viz::VtkVizData::insertIntoMultiBlock(vtkSmartPointer<vtkMultiBlockDataSet> mbToFill, unsigned int blockno) {
    mbToFill->SetBlock(blockno, mb_);
    mbToFill->GetMetaData(blockno)->Set( vtkCompositeDataSet::NAME(), name_.c_str());
}
