// Copyright (c) 2011 LTSI INSERM U642
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
// 
//     * Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation and/or
// other materials provided with the distribution.
//     * Neither name of LTSI, INSERM nor the names
// of any contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "vtkHausdorffDistanceOctreesFilter.h"


#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>

#include <vtkSmartPointer.h>
#include <vtkPointSet.h>
#include <vtkKdTreePointLocator.h>
#include <vtkCellLocator.h>
#include <vtkGenericCell.h>

#include <cmath>
#include <omp.h>
#include <pthread.h>

vtkStandardNewMacro(vtkHausdorffDistanceOctreesFilter);


vtkHausdorffDistanceOctreesFilter::vtkHausdorffDistanceOctreesFilter()
{
  this->RelativeDistance[0]=0.0;
  this->RelativeDistance[1]=0.0;
  this->HausdorffDistance = 0.0;
  
  this->SetNumberOfInputPorts(2);
  this->SetNumberOfOutputPorts(2);
  
  this->TargetDistanceMethod = POINT_TO_POINT;
}

vtkHausdorffDistanceOctreesFilter::~vtkHausdorffDistanceOctreesFilter()
{
}

int vtkHausdorffDistanceOctreesFilter::RequestData(vtkInformation *vtkNotUsed(request),
		vtkInformationVector **inputVector,
		vtkInformationVector *outputVector)
{
  //This function calls the scanners input and output to allow it to
  //work in the vtk algorithm pipeline

  // get the info objects
  //vtkInformation **inInfo=new vtkInformation * [2];
  vtkInformation* inInfoA = inputVector[0]->GetInformationObject(0);
  vtkInformation* inInfoB = inputVector[1]->GetInformationObject(0);
  vtkInformation* outInfoA = outputVector->GetInformationObject(0);
  vtkInformation* outInfoB = outputVector->GetInformationObject(1);

  // get the input
  vtkPointSet* inputA = vtkPointSet::SafeDownCast(
			inInfoA->Get(vtkDataObject::DATA_OBJECT()));
  vtkPointSet* inputB = vtkPointSet::SafeDownCast(
			inInfoB->Get(vtkDataObject::DATA_OBJECT()));
  vtkPointSet* outputA = vtkPointSet::SafeDownCast(
			outInfoA->Get(vtkDataObject::DATA_OBJECT()));
  vtkPointSet* outputB = vtkPointSet::SafeDownCast(
			outInfoB->Get(vtkDataObject::DATA_OBJECT()));


  if( inputA->GetNumberOfPoints() == 0 || inputB->GetNumberOfPoints() == 0 )
	return( 0 );

  // Re-initialize the distances 
  this->RelativeDistance[0]=0.0;
  this->RelativeDistance[1]=0.0;
  this->HausdorffDistance = 0.0;

  
  double dist;
  vtkIdType cellId;
  vtkSmartPointer<vtkGenericCell> cell = vtkSmartPointer<vtkGenericCell>::New( );
  int subId;

  vtkSmartPointer<vtkDoubleArray> distanceAToB = vtkSmartPointer<vtkDoubleArray>::New();
  distanceAToB->SetNumberOfComponents(1);
  distanceAToB->SetNumberOfTuples(inputA->GetNumberOfPoints());
  distanceAToB->SetName( "Distance" );
      
  vtkSmartPointer<vtkDoubleArray> distanceBToA = vtkSmartPointer<vtkDoubleArray>::New();
  distanceBToA->SetNumberOfComponents(1);
  distanceBToA->SetNumberOfTuples(inputB->GetNumberOfPoints());
  distanceBToA->SetName( "Distance" );

  vtkSmartPointer<vtkKdTreePointLocator> pointLocatorA = vtkSmartPointer<vtkKdTreePointLocator>::New();
  vtkSmartPointer<vtkKdTreePointLocator> pointLocatorB = vtkSmartPointer<vtkKdTreePointLocator>::New();
  vtkSmartPointer<vtkCellLocator> cellLocatorA = vtkSmartPointer<vtkCellLocator>::New();
  vtkSmartPointer<vtkCellLocator> cellLocatorB = vtkSmartPointer<vtkCellLocator>::New();  
  pointLocatorA->SetDataSet(inputA);
  pointLocatorB->SetDataSet(inputB);
  cellLocatorA->SetDataSet(inputA);  
  cellLocatorB->SetDataSet(inputB);

  omp_set_num_threads(1);
  
  //#pragma omp parallel
  //{    
    
    if (this->TargetDistanceMethod == POINT_TO_POINT) {
      pointLocatorA->BuildLocator();
      pointLocatorB->BuildLocator();
      std::cout << "POINT_TO_POINT" << std::endl;
    } else {
      cellLocatorA->BuildLocator();
      cellLocatorB->BuildLocator();
      std::cout << "POINT_TO_CELL" << std::endl;
    }

    std::cout << "Start loop... with Points=" << inputA->GetNumberOfPoints() << std::endl;
    #pragma omp parallel for 
    for (int i = 0; i < inputA->GetNumberOfPoints(); i++) {
      //int myID=omp_get_thread_num ();
      //std::cout << "[ "<< myID <<" ]" << " = " << i << std::endl;      
      int N = 1;
      double currentPoint[3];
      double closestPoint[3];      
      inputA->GetPoint(i, currentPoint);
      if (this->TargetDistanceMethod == POINT_TO_POINT ) { 
	vtkNew<vtkIdList> points;
	pointLocatorB->FindClosestNPoints(N, currentPoint, points);
	vtkIdType closestPointId = points->GetId(0);      
	//vtkIdType closestPointId = pointLocatorB->FindClosestPoint(currentPoint);	
	inputB->GetPoint(closestPointId, closestPoint);
      } else {
	cellLocatorB->FindClosestPoint(currentPoint,closestPoint,cell,cellId,subId,dist);
      }
      dist = sqrt(pow(currentPoint[0]-closestPoint[0],2)+pow(currentPoint[1]-closestPoint[1],2)+pow(currentPoint[2]-closestPoint[2],2));	  
      distanceAToB->SetValue(i,dist);
      if( dist > this->RelativeDistance[0]) {
	this->RelativeDistance[0] = dist;
      }
    }
    //}

  //if(this->RelativeDistance[0] >= RelativeDistance[1])
  this->HausdorffDistance = this->RelativeDistance[0];
  //else
  //this->HausdorffDistance = this->RelativeDistance[1];


  vtkSmartPointer<vtkDoubleArray> relativeDistanceAtoB = vtkSmartPointer<vtkDoubleArray>::New( );
  relativeDistanceAtoB->SetNumberOfComponents( 1 );
  relativeDistanceAtoB->SetName( "RelativeDistanceAtoB" );
  relativeDistanceAtoB->InsertNextValue( RelativeDistance[0] );
 
  vtkSmartPointer<vtkDoubleArray> hausdorffDistanceFieldDataA = vtkSmartPointer<vtkDoubleArray>::New( );
  hausdorffDistanceFieldDataA->SetNumberOfComponents( 1 );
  hausdorffDistanceFieldDataA->SetName( "HausdorffDistance" );
  hausdorffDistanceFieldDataA->InsertNextValue( HausdorffDistance );

  outputA->DeepCopy( inputA );
  outputA->GetPointData( )->AddArray( distanceAToB );
  outputA->GetFieldData( )->AddArray( relativeDistanceAtoB );
  outputA->GetFieldData( )->AddArray( hausdorffDistanceFieldDataA );

  return 1;
}



int vtkHausdorffDistanceOctreesFilter::FillInputPortInformation( int port, vtkInformation* info )
{

	//The input should be two vtkPolyData
	if (port == 0)
	{
		info->Set( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet" );
		return 1;
	}
	if (port == 1)
	{
		info->Set( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet" );
		return 1;
	}

	return 0;
}

int vtkHausdorffDistanceOctreesFilter::FillOutputPortInformation( int port, vtkInformation* info )
{

	//The input should be two vtkPolyData
	if (port == 0)
	{
		info->Set( vtkDataObject::DATA_TYPE_NAME(), "vtkPointSet" );
		return 1;
	}
	if (port == 1)
	{
		info->Set( vtkDataObject::DATA_TYPE_NAME(), "vtkPointSet" );
		return 1;
	}

	return 0;
}

void vtkHausdorffDistanceOctreesFilter::PrintSelf(ostream &os, vtkIndent indent)
{
	//print the scanners information when << is called
	os << "Output" << std::endl
			<< "-----------" << std::endl;

}

