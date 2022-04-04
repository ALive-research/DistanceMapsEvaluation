/*===============================================================================

  Project: ITKDistanceMaps
  Module: itkObjectToObjectDistanceMap.h

  Copyright (c) 2020,  Oslo University Hospital and University of Córdoba.

  All rights reserved. This is propietary software. In no event shall the author
  be liable for any claim or damages.

  ===============================================================================*/

#ifndef __itkObjectToObjectDistanceMap_h
#define __itkObjectToObjectDistanceMap_h
 
#include "itkImageToImageFilter.h"
#include "itkPointSet.h"
 
namespace itk
{
  template< class TImage>
    class itkObjectToObjectDistanceMap:public ImageToImageFilter< TImage, TImage >
    {
    public:
      /** Standard class typedefs. */
      typedef itkObjectToObjectDistanceMap         Self;
      typedef ImageToImageFilter< TImage, TImage > Superclass;
      typedef SmartPointer< Self >                 Pointer;
 
      /** Method for creation through the object factory. */
      itkNewMacro(Self);
 
      /** Run-time type information (and related methods). */
      itkTypeMacro(itkObjectToObjectDistanceMap, ImageToImageFilter);
 
      typedef typename Superclass::OutputImageRegionType OutputImageRegionType;

      // Type of data necesary for the attribute '_interestPointSet'.
      typedef itk::PointSet< TImage, TImage::ImageDimension> PointSetType;

      // Function to add a vector as interest values. This vector contains the
      // values of the target voxels to compute the distance.
      void SetPointOfInterestValues( std::vector<int> values );

      // Function to get the interest points stored in a stl vector.
      std::vector<int> GetPointOfInterestValues();
 
      // Function to get the number of the diferent voxel values stored in the
      // vector of interest points.
      int GetSizePointOfInterestValues();

      // Function to add an interest point.
      int AddInterestValue(int value);

      // Function to specify the value of the background. By defult this value is '0'.
      void SetBackgroundValue( int value );

      // Function to get the background value.
      int GetBackgroundValue();

      // Function to specify if the distance from the background to the interest
      // voxels will be calculated or not. By default this value is 'false'.
      void SetComputeBackgroundDistance( bool value );

      // Function to know if the distance from the background to the interest
      // voxels will be calculated or not.
      bool GetComputeBackgroundDistance();

    protected:
      itkObjectToObjectDistanceMap()
      {
         _backgroudValue = 0;
         _computeDistanceBackgroundToPointOfInterestValues = false;
         this->DynamicMultiThreadingOff();
       }
      ~itkObjectToObjectDistanceMap(){}
 
      // Function to preprocess data in a single thread before call
      // ThreadedGenerateData.
      virtual void BeforeThreadedGenerateData();

      // Main function of the filter. It does the calculate distance of image.
      virtual void ThreadedGenerateData(const OutputImageRegionType &, ThreadIdType);
 
    private:
      itkObjectToObjectDistanceMap(const Self &); //purposely not implemented
      void operator=(const Self &);  //purposely not implemented

      // labels of interest points to compute the distance to them.
      std::vector<int> _pointsOfInterestValues;

      // background label value of an image.
      int _backgroudValue;

      // variable to know if the distance from background to interest values
      // will be calculated or not.
      bool _computeDistanceBackgroundToPointOfInterestValues; 

      // This array contains the voxels values to compute the minimun distance
      // of each voxel of the image to them.
      typename PointSetType::Pointer _interestPointSet; 
 
    };

} //namespace itk
 
 
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkObjectToObjectDistanceMap.hxx"
#endif
 
 
#endif // __itkObjectToObjectDistanceMap_h
