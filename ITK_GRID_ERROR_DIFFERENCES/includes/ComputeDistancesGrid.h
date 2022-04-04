#ifndef __ComputeDistancesGrid_h
#define __ComputeDistancesGrid_h
 
#include "itkImageToImageFilter.h"
#include "itkPointSet.h"
#include <vector> 

double gridTime = 0.0;

namespace itk
{
	template< class TImage>
	class ComputeDistancesGrid:public ImageToImageFilter< TImage, TImage >
	{
	public:
	  /** Standard class typedefs. */
	  typedef ComputeDistancesGrid      Self;
	  typedef ImageToImageFilter< TImage, TImage > Superclass;
	  typedef SmartPointer< Self >                 Pointer;

    // Type of data necesary for the attribute '_interestPointSet'.
    typedef itk::PointSet< TImage, TImage::ImageDimension> PointSetType;
 
	  /** Method for creation through the object factory. */
	  itkNewMacro(Self);
 
	  /** Run-time type information (and related methods). */
	  itkTypeMacro(ComputeDistancesGrid, ImageToImageFilter);
 
		// Function to add a vector as interest values. This vector contains the values of the target voxels to compute the distance.
	  void SetPointOfInterestValues( std::vector<int> values );

		// Function to get the interest points stored in a stl vector.
	  std::vector<int> GetPointOfInterestValues();

		// Function to get the number of the diferent voxel values stored in the vector of interest points.
	  int GetSizePointOfInterestValues();

		// Function to add an interest point.
	  int AddInterestValue(int value);

		// Function to specify the value of the background. By defult this value is '0'.
	  void SetBackgroundValue( int value );

		// Function to get the background value.
	  int GetBackgroundValue();

		// Function to specify if the distance from the background to the interest voxels will be calculated or not. By default this value is 'false'.
	  void SetComputeBackgroundDistance( bool value );

		// Function to know if the distance from the background to the interest voxels will be calculated or not.
	  bool GetComputeBackgroundDistance();

		// Function to set the percentage of reduction of amount points in the grid that will be generated.
	  void SetPercentageOfPointsInGrid( float value );

		// Function to get the percentage of reduction of amount points in the grid that will be generated.
	  float GetPercentageOfPointsInGrid();

		// Function to specify the distance beetween points in the grid in the x,y and z axis. It will be calculated automatically. However, we can select the distance manually.
		void SetGridDimension(int x_dim, int y_dim, int z_dim);

		// Function to get the distance beetween points in the grid for the x,y and z axis.
		int* GetGridDimension();

	protected:
	  ComputeDistancesGrid()
	  {
	    _backgroudValue = 0;
	    _computeDistanceBackgroundToPointOfInterestValues = false;
			_percentageOfPointsInGrid = 1; // '1' is the whole image, This value can be adjusted until 0 where no points will be available.
//			_gridPointSpacing[0] = _gridPointSpacing[1] = _gridPointSpacing [2] = -1; // Initialized to -1. It will never be negative. Hence, these values mean that the corret ones will be calculated automatically.
      _interestPointSet = PointSetType::New();
  	}
	
	  ~ComputeDistancesGrid(){}
 
	  /** Function to generate the grid. */
	  virtual void GenerateData();
 
	private:
	  ComputeDistancesGrid(const Self &); //purposely not implemented
	  void operator=(const Self &);  //purposely not implemented

  	std::vector<int> _pointsOfInterestValues; // labels of interest points to compute the distance to them.
	  int _backgroudValue; // background label value of an image.
	  bool _computeDistanceBackgroundToPointOfInterestValues; // variable to know if the distance from background to interest values will be calculated or not.
		float _percentageOfPointsInGrid; // Percentage of the points from the original amount of points that will used as index to compute the distances. This is for generate a grid of N points with the same distance which covers the original physical extention of the image. This percentage is used to know the number of points in the grid.
		int _gridDimensionSize[3]; // These values are choosed by the user and specify the pixel dimension of the generated grid. 

    typename PointSetType::Pointer _interestPointSet; // This array contains the voxels values to compute the minimun distance of each voxel of the image to them.
	};
} //namespace ITK
 
 
#ifndef ITK_MANUAL_INSTANTIATION
#include "ComputeDistancesGrid.hxx"
#endif
 
 
#endif // __ComputeDistancesGrid_h
