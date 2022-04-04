/*===============================================================================

  Project: ITKDistanceMaps
  Module: itkObjectToObjectDistanceMap.hxx

  Copyright (c) 2020,  Oslo University Hospital and University of Córdoba.

  All rights reserved. This is propietary software. In no event shall the author
  be liable for any claim or damages.

  ===============================================================================*/

#ifndef __itkObjectToObjectDistanceMap_hxx
#define __itkObjectToObjectDistanceMap_hxx
 
#include "itkObjectToObjectDistanceMap.h" 

#include "itkObjectFactory.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkPointSet.h"

#include <limits>

namespace itk
{
  // Function to add a vector as interest values. This vector contains
  // the values of the target voxels to compute the distance.
  template< class TImage> 
  void itkObjectToObjectDistanceMap< TImage>
  ::SetPointOfInterestValues( std::vector<int> values )
  {
    _pointsOfInterestValues = values;
  }

  // Function to get the interest points stored in a stl vector.
  template< class TImage> 
  std::vector<int> itkObjectToObjectDistanceMap< TImage>
  ::GetPointOfInterestValues()
  {
    return _pointsOfInterestValues;
  }

  // Function to get the number of the diferent voxel values stored in
  // the vector of interest points.
  template< class TImage> 
  int itkObjectToObjectDistanceMap< TImage>
  ::GetSizePointOfInterestValues()
  {
    return (int)_pointsOfInterestValues.size();
  }

  // Function to add an interest point.
  template< class TImage> 
  int itkObjectToObjectDistanceMap< TImage>
  ::AddInterestValue(int value)
  {
    _pointsOfInterestValues.push_back(value);
    return (int)_pointsOfInterestValues.size();
  }

  // Function to specify the value of the background. By defult this
  // value is '0'.
  template< class TImage> 
  void itkObjectToObjectDistanceMap< TImage>
  ::SetBackgroundValue( int value )
  {
    _backgroudValue = value;
  }
  
  // Function to get the background value.
  template< class TImage> 
  int itkObjectToObjectDistanceMap< TImage>
  ::GetBackgroundValue()
  {
    return _backgroudValue;
  }

  // Function to specify if the distance from the background to the
  // interest voxels will be calculated or not. By default this value
  // is 'false'.
  template< class TImage> 
  void itkObjectToObjectDistanceMap< TImage>
  ::SetComputeBackgroundDistance( bool value )
  {
    _computeDistanceBackgroundToPointOfInterestValues = value;
  }

  // Function to know if the distance from the background to the
  // interest voxels will be calculated or not.
  template< class TImage> 
  bool itkObjectToObjectDistanceMap< TImage>
  ::GetComputeBackgroundDistance()
  {
    return _computeDistanceBackgroundToPointOfInterestValues;
  }


  // Function to preprocess the image before split the image into
  // threads.
  template< class TImage>
  void itkObjectToObjectDistanceMap< TImage>
  ::BeforeThreadedGenerateData()
  {
    typename TImage::ConstPointer input = this->GetInput();
    typename TImage::Pointer output = this->GetOutput();
    typename itk::PointSet< TImage, TImage::ImageDimension>::PointType PointType;
    typename itk::PointSet< TImage, TImage::ImageDimension>::PointType point;
    typename TImage::RegionType region;
    typename TImage::SizeType regionSize;

    _interestPointSet = PointSetType::New();


    unsigned int pointId = 0; // point id to save all diferents voxel indexes in an itk array
    double dist = 0.0; // Minimum distance calculated.
    double newDist = 0.0; // Distance calculated in every step.
    int Neighborhood; // voxel value of a neighborhood voxel.
    int voxelValue; // value of a voxel that we are checking.
    int cont = 0; // counter for loops
    size_t n = 0; // counter for loops

    // get the region size of the image
    region = input->GetBufferedRegion();
    regionSize = region.GetSize();

    // show region size information
    //std::cout << regionSize << std::endl;

    // Allocate size in memory for the output image
    this->AllocateOutputs();

    // Iterators for input/output images 
    itk::ImageRegionIterator<TImage> outputImageIterator(output,region);

    typename TImage::SizeType radius;
    radius[0] = 1;
    radius[1] = 1;
    radius[2] = 1;
    // radius.Fill(1); // this is equivalent to 3x3x3 neigborhood

    // Iterator for check all the neighborhood of a voxel
    itk::ConstNeighborhoodIterator<TImage> iteratorNeighborhood(radius, input,region); 

    // First loop to find all elements which correspond with an
    // interest region
    for (iteratorNeighborhood.GoToBegin(),outputImageIterator.GoToBegin();
         !iteratorNeighborhood.IsAtEnd();
         ++iteratorNeighborhood, ++outputImageIterator)
    {
       voxelValue = (int)iteratorNeighborhood.GetCenterPixel();

       // by default all voxels have a nevative distance. '-2' means that
       // distance needs to be calculated in next steps.
       outputImageIterator.Set(-2);

        // check all voxels that are not a background
        if(voxelValue != _backgroudValue)
        {
          // check inside all point of interest
          for(cont = 0; cont < (int)_pointsOfInterestValues.size(); ++cont) 
          {
            // if is a point of interest check its neighborhood
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
              // Since this is an interest point, its distance
              // is 0. The distance is 0 for edge points and
              // points inside an interest region.
              outputImageIterator.Set(0);
            }
          }
        }
        else
        {
          // If we select to no calculate the distance to the
          // background points, all the values of background has -1
          // as distance.
          if(!_computeDistanceBackgroundToPointOfInterestValues)
            outputImageIterator.Set(-1);

          // result of distance at this point is -1
        }
    }
    
    //At the end of this loop we have the output image filled with -2
    //(points to compute distance), -1(point of background if
    //_computeDistanceBackgroundToPointOfInterestValues == false),
    //0(tumor points or points inside the tumor).

    itkDebugMacro("Number Of Interest Points = "
      << _interestPointSet->GetNumberOfPoints());
  }


  // Main function of the filter to compute the distances of each voxel of an image.
  template< class TImage>
  void itkObjectToObjectDistanceMap< TImage>
  ::ThreadedGenerateData(const OutputImageRegionType & region, ThreadIdType threadId)
  {
    typename TImage::ConstPointer input = this->GetInput();
    typename TImage::Pointer output = this->GetOutput();
    typename TImage::SizeType regionSize;
    typename itk::PointSet< TImage, TImage::ImageDimension>::PointType PointType;
    typename itk::PointSet< TImage, TImage::ImageDimension>::PointType point0;
    typename itk::PointSet< TImage, TImage::ImageDimension>::PointType point;
    unsigned int pointId = 0; // point id to save all diferents voxel indexes in an itk array
    float dist = 0.0; // Minimum distance calculated.
    float newDist = 0.0; // Distance calculated in every step.
    int voxelValue; // value of a voxel that we are checking.
    unsigned int i = 0; // counter for loops

    // Information about each thread and the region of the image that will compute.
    //std::cout << "\nThread " << threadId << " given region: " << region << std::endl;

    // Iterators for input/output images 
    itk::ImageRegionConstIterator<TImage> inputImageIterator(input,region);
    itk::ImageRegionIterator<TImage> outputImageIterator(output,region);

    // The Second loop calculates the distance of every point of the
    // image to a tumor and it stores in a new image the shortest
    // distance.
    for (inputImageIterator.GoToBegin(), outputImageIterator.GoToBegin();
   !inputImageIterator.IsAtEnd(); ++inputImageIterator, ++outputImageIterator)
      {
         // get the voxel value of each voxel in the image
         voxelValue = (int)outputImageIterator.Get(); 

         // if the distance is -2 it is necesary to calculate the
         // distance. Otherwise, we do not need to compute the
         // distance.
         if(voxelValue == -2)
         {
           // the first distance always has an infinite value
           dist = std::numeric_limits<float>::max();

           // Transform from index to physical point.
           input->TransformIndexToPhysicalPoint( inputImageIterator.GetIndex() , point0);
  
           // Find the minimum distance to all the points in an array
           // of interests points
           for(i = 0; i < _interestPointSet->GetNumberOfPoints(); ++i)
           {
             _interestPointSet->GetPoint(i, &point);
             newDist = point0.EuclideanDistanceTo(point);

             if(newDist<dist)
               dist = newDist;
           }
      
           // Store the closest distance in the distance field image
           outputImageIterator.Set(dist);
         }
      }
  }
 
}// end namespace
 
 
#endif // __itkObjectToObjectDistanceMap_h
