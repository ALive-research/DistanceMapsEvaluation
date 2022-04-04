#ifndef __ComputeDistancesGrid_hxx
#define __ComputeDistancesGrid_hxx
 
#include "itkObjectFactory.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkPointSet.h"

#include "itkVector.h"
#include "itkMath.h"
#include "itkListSample.h"
#include "itkWeightedCentroidKdTreeGenerator.h"
#include "itkEuclideanDistanceMetric.h"

#include "itkTimeProbe.h"

#include "ComputeDistancesGrid.h" 

namespace itk
{
// Function to add a vector as interest values. This vector contains the values of the target voxels to compute the distance.
template< class TImage>
void ComputeDistancesGrid< TImage>
::SetPointOfInterestValues( std::vector<int> values )
{
  _pointsOfInterestValues = values;
}

// Function to get the interest points stored in a stl vector.
template< class TImage> 
std::vector<int> ComputeDistancesGrid< TImage>
::GetPointOfInterestValues()
{
  return _pointsOfInterestValues;
}

// Function to get the number of the diferent voxel values stored in the vector of interest points.
template< class TImage> 
int ComputeDistancesGrid< TImage>
::GetSizePointOfInterestValues()
{
  return (int)_pointsOfInterestValues.size();
}

// Function to add an interest point.
template< class TImage> 
int ComputeDistancesGrid< TImage>
::AddInterestValue(int value)
{
  _pointsOfInterestValues.push_back(value);
  return (int)_pointsOfInterestValues.size();
}

// Function to specify the value of the background. By defult this value is '0'.
template< class TImage> 
void ComputeDistancesGrid< TImage>
::SetBackgroundValue( int value )
{
  _backgroudValue = value;
}
  
// Function to get the background value.
template< class TImage> 
int ComputeDistancesGrid< TImage>
::GetBackgroundValue()
{
  return _backgroudValue;
}

// Function to specify if the distance from the background to the interest voxels will be calculated or not. By default this value is 'false'.
template< class TImage> 
void ComputeDistancesGrid< TImage>
::SetComputeBackgroundDistance( bool value )
{
  _computeDistanceBackgroundToPointOfInterestValues = value;
}

// Function to know if the distance from the background to the interest voxels will be calculated or not.
template< class TImage> 
bool ComputeDistancesGrid< TImage>
::GetComputeBackgroundDistance()
{
  return _computeDistanceBackgroundToPointOfInterestValues;
}

// Function to set the percentage of reduction of amount points in the grid that will be generated.
template< class TImage> 
void ComputeDistancesGrid< TImage>
::SetPercentageOfPointsInGrid( float value )
{
  _percentageOfPointsInGrid = value;
}

// Function to get the percentage of reduction of amount points in the grid that will be generated.
template< class TImage> 
float ComputeDistancesGrid< TImage>
::GetPercentageOfPointsInGrid()
{
  return _percentageOfPointsInGrid;
}

// Function to specify the distance beetween points in the grid in the x,y and z axis. It will be calculated automatically. However, we can select the distance manually.
template< class TImage> 
void ComputeDistancesGrid< TImage>
::SetGridDimension( int x_dim, int y_dim, int z_dim )
{
  _gridDimensionSize[0] = x_dim;
  _gridDimensionSize[1] = y_dim;
  _gridDimensionSize[2] = z_dim;
}
// Function to get the distance beetween points in the grid for the x,y and z axis.
template< class TImage> 
int * ComputeDistancesGrid< TImage>
::GetGridDimension()
{
  return _gridDimensionSize;
}


// This is the function which it called in a single thread before that generate the grid and the octrees. The structures generated in this function will be accesed by all the threads in the next function.
template< class TImage>
void ComputeDistancesGrid< TImage>
::GenerateData()
{
typename TImage::ConstPointer input = this->GetInput();
typename TImage::Pointer output = this->GetOutput();
typename TImage::RegionType region, GridRegion;
typename TImage::SizeType regionSize;
typename TImage::SpacingType sp;
typename TImage::PointType origin;
typename TImage::DirectionType direct;
typename itk::PointSet< TImage, TImage::ImageDimension>::PointType testPoint;
typename itk::PointSet< TImage, TImage::ImageDimension>::PointType point;

  itk::TimeProbe clock;
  clock.Start();

unsigned int pointId = 0; // point id to save all diferents voxel indexes in an itk array
double auxPoint[3]; // auxiliary point.
int Neighborhood; // voxel value of a neighborhood voxel.
int voxelValue; // value of a voxel that we are checking.
int cont = 0; // counter for loops
size_t n = 0; // counter for loops


	// get the region size of the image
  region = input->GetBufferedRegion();
  regionSize = region.GetSize();

  // Allocate size in memory for the output image
//  this->AllocateOutputs();


  sp = input->GetSpacing();
  origin = input->GetOrigin();
  direct = input->GetDirection();
	// show region size information
//  std::cout << "RegionSize = " << regionSize[0] << ", " << regionSize[1] << ", " << regionSize[2] << std::endl;
//  std::cout << "Spacing = " << sp[0] << ", " << sp[1] << ", " << sp[2] << std::endl;
//  std::cout << "Origin = " << origin[0] << ", " << origin[1] << ", " << origin[2] << std::endl;
//  std::cout << "Direction = " << std::endl;
//  std::cout << direct << std::endl;
//  std::cout << "Physical extension = " << regionSize[0] * (float)sp[0] << ", " << regionSize[1] * (float)sp[1] << ", " << regionSize[2] * (float)sp[2] << std::endl;

	//Grid Generation
	//---------------

	// calculate the physical extension of the image. It is calculated as: physicalExtension = regionSize (number of voxels) * spacing (separation of the voxels).
	float physicalExten[3] = {regionSize[0] * (float)sp[0], regionSize[1] * (float)sp[1], regionSize[2] * (float)sp[2]};

//	float spacing_counterX, spacing_counterY, spacing_counterZ;// Variables used to know the new separation of the points in the grid to cover all the physical extesion of the image.

	// the spacing along each axis is the division between: the total spacing / new amount of points.
	// the conition is to check if the user specified or not the spacing for each axis, because by default it is set to '-1'. 
//	if(_gridPointSpacing[0] ==-1 && _gridPointSpacing[1] ==-1 && _gridPointSpacing[2] ==-1)
//	{
//		_gridPointSpacing[0] = (physicalExten[0]/(regionSize[0] * _percentageOfPointsInGrid));
//		_gridPointSpacing[1] = (physicalExten[1]/(regionSize[1] * _percentageOfPointsInGrid));
//		_gridPointSpacing[2] = (physicalExten[2]/(regionSize[2] * _percentageOfPointsInGrid));
//	}

typename TImage::IndexType start;
 
  start[0] = 0; // first index on X
  start[1] = 0; // first index on Y
  start[2] = 0; // first index on Z



typename TImage::SizeType size;
 
  size[0] = _gridDimensionSize[0]; // size along X
  size[1] = _gridDimensionSize[1]; // size along Y
  size[2] = _gridDimensionSize[2]; // size along Z


GridRegion.SetSize(size);
GridRegion.SetIndex(start);


float gridSpacing[3]= {(physicalExten[0]/(float)_gridDimensionSize[0]), (physicalExten[1]/(float)_gridDimensionSize[1]), (physicalExten[2]/(float)_gridDimensionSize[2])};

typename TImage::PointType originGrid;

originGrid[0] = (origin[0] - ((float)sp[0]/2.0 )) + (gridSpacing[0] / 2.0);
originGrid[1] = (origin[1] - ((float)sp[1]/2.0 )) + (gridSpacing[1] / 2.0);
originGrid[2] = (origin[2] - ((float)sp[2]/2.0 )) + (gridSpacing[2] / 2.0);

  output->SetRegions(GridRegion);
  output->SetOrigin(originGrid);
  output->SetSpacing(gridSpacing);
  output->SetDirection(direct);

  output->Allocate(true); // initialize buffer to zero
 clock.Stop();

  typename TImage::SizeType radius;
  radius[0] = 1;
  radius[1] = 1;
  radius[2] = 1;
  // radius.Fill(1); // this is equivalent to 3x3x3 neigborhood

  // Iterator for check all the neighborhood of a voxel
  itk::ConstNeighborhoodIterator<TImage> iteratorNeighborhood(radius, input, region); 

  // First loop to find all elements which correspond with an
  // interest region
  for (iteratorNeighborhood.GoToBegin(); !iteratorNeighborhood.IsAtEnd(); ++iteratorNeighborhood)
  {
     voxelValue = (int)iteratorNeighborhood.GetCenterPixel();

     // check inside all point of interest
     for(cont = 0; cont < (int)_pointsOfInterestValues.size(); ++cont) 
     {
       // if it is a point of interest check its neighborhood
       if(voxelValue == _pointsOfInterestValues[cont]) 
       {
         // loop to check all the neighbourds of an interest point.
         for (n = 0; n < iteratorNeighborhood.Size(); ++n) 
         {
           Neighborhood = iteratorNeighborhood.GetPixel(n);
        
           // If the value of a neighborhood is different
           // to the interest point, it is a edge point
           // and save inside an array
           if(Neighborhood != _pointsOfInterestValues[cont])
           {
             // Convert the pixel position into a Point
             input->TransformIndexToPhysicalPoint( iteratorNeighborhood.GetIndex(), point);
  
             _interestPointSet->SetPoint( pointId, point );
             ++pointId; 

             // stop the point of interest search list loop
             cont = (int)_pointsOfInterestValues.size(); 
             n = iteratorNeighborhood.Size(); // stop the neighborhood search loop
           }
         }
       }
     }
  }

//  std::cout<<"Number Of Interest Points = " << _interestPointSet->GetNumberOfPoints()<< std::endl;



  clock.Start();

typename itk::PointSet< TImage, TImage::ImageDimension>::PointType AuxPoint;
itk::ImageRegionIterator<TImage> outputImageIterator(output,GridRegion);
float dist = 0.0; // Minimum distance calculated.
float newDist = 0.0; // Distance calculated in every step.
unsigned int i = 0; // counter for loops
//int cont2=0;


// BRUTE FORCE CODE:
//------------------  
/*
	// The Second loop calculates the distance of every point of the image to a tumor and it stores in a new image the shortest distance.
	for (outputImageIterator.GoToBegin(); !outputImageIterator.IsAtEnd(); ++outputImageIterator)
	{
		// the first distance always has an infinite value
		dist = std::numeric_limits<float>::max();

		// Transform from index to physical point.
		output->TransformIndexToPhysicalPoint( outputImageIterator.GetIndex(), AuxPoint);

		// Find the minimum distance to all the points in an array of interests points
		for(i = 0; i < _interestPointSet->GetNumberOfPoints(); ++i)
		{
			_interestPointSet->GetPoint(i, &point);
			newDist = point.EuclideanDistanceTo(AuxPoint);

			if(newDist<dist)
				dist = newDist;     
		}

			//std::cout<<"\ndist: "<<dist<<"\n";
	  // Store the closest distance in the distance field image
		outputImageIterator.Set((float)dist);
//outputImageIterator.Set(cont2/10000.0);
//++cont2;
	}
*/

// KD-TREE CODE:
//--------------
using MeasurementVectorType = itk::Vector<float, TImage::ImageDimension>;
using SampleType = itk::Statistics::ListSample<MeasurementVectorType>;

 typename SampleType::Pointer sample = SampleType::New();
  sample->SetMeasurementVectorSize(3);

  MeasurementVectorType mv;
  for(i = 0; i < _interestPointSet->GetNumberOfPoints(); ++i)
  {
    _interestPointSet->GetPoint(i, &point);
    mv[0] = point[0];
    mv[1] = point[1];
    mv[2] = point[2];
    sample->PushBack(mv);
  }

  using TreeGeneratorType = itk::Statistics::KdTreeGenerator<SampleType>;
  typename TreeGeneratorType::Pointer treeGenerator = TreeGeneratorType::New();
 
  treeGenerator->SetSample(sample);
  treeGenerator->SetBucketSize(16);
  treeGenerator->Update();

  using TreeType = typename TreeGeneratorType::KdTreeType;
  using  NodeType = typename TreeType::KdTreeNodeType;
  typename TreeType::Pointer tree = treeGenerator->GetOutput();
  MeasurementVectorType queryPoint;

  using DistanceMetricType = itk::Statistics::EuclideanDistanceMetric<MeasurementVectorType>;
  typename DistanceMetricType::Pointer distanceMetric = DistanceMetricType::New();
  typename DistanceMetricType::OriginType originKD(3);
  unsigned int numberOfNeighbors = 1;
  typename TreeType::InstanceIdentifierVectorType neighbors;

	for (outputImageIterator.GoToBegin(); !outputImageIterator.IsAtEnd(); ++outputImageIterator)
	{
		// Transform from index to physical point.
           output->TransformIndexToPhysicalPoint( outputImageIterator.GetIndex(), AuxPoint);

           queryPoint[0] = AuxPoint[0];
           queryPoint[1] = AuxPoint[1];
           queryPoint[2] = AuxPoint[2];

           for (i = 0; i < sample->GetMeasurementVectorSize(); ++i)
           {
              originKD[i] = queryPoint[i];
           }
           distanceMetric->SetOrigin(originKD);

           tree->Search(queryPoint, numberOfNeighbors, neighbors);

  	  // Store the closest distance in the distance field image
          outputImageIterator.Set((float)distanceMetric->Evaluate(tree->GetMeasurementVector(neighbors[0])));
        }

 clock.Stop();


//  std::cout << "Mean: " << clock.GetMean() << std::endl;
//  std::cout << "Total: " << clock.GetTotal() << std::endl;
gridTime = clock.GetTotal(); // guardar tiempo en variable global
//  std::cout<<"\nTask Done!\n";

}

}// end namespace
 
 
#endif // __ComputeDistancesGrid_hxx
