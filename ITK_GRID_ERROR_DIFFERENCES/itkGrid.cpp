#include <iostream> // For standar input/output
#include <itkImage.h> // For managing images
#include <itkImageFileReader.h> // For reading Images
#include "itkImageFileWriter.h" // For writing Images
#include "itkPoint.h" // For managing points
#include <itkNrrdImageIO.h> // For managing .nrrd images


#include "itkTimeProbe.h"
#include "includes/ComputeDistancesGrid.h"


#include "itkListSample.h"
#include "itkWeightedCentroidKdTreeGenerator.h"
#include "itkEuclideanDistanceMetric.h"

int main(int argc, char *argv[])
{
  itk::TimeProbe clock;
//  itk::MemoryProbe memoryProbe;
// memoryProbe.Start(); 
//  clock.Start();
  if( argc < 5 )
  {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputImageBruteForceFile inputImageFile outputImageFileName gridDimension" << std::endl;
    return EXIT_FAILURE;
  }

  using PixelType = float;
  using ImageType = itk::Image<PixelType, 3>;
  using FileReaderType = itk::ImageFileReader<ImageType, itk::DefaultConvertPixelTraits< PixelType > >;
  using ReaderType = itk::ImageFileReader<ImageType>;
  using ComputeDistancesGridFilter = itk::ComputeDistancesGrid<ImageType>;

  ReaderType::Pointer reader = ReaderType::New();
  ImageType::Pointer image = ImageType::New();

  reader->SetFileName(argv[2]);
  reader->Update();
  image = dynamic_cast<ImageType *>(reader->GetOutput());


  ImageType::SizeType regionSize;
  ImageType::RegionType region;

  region = image->GetLargestPossibleRegion();
  regionSize = region.GetSize();

  // Create and the filter
  ComputeDistancesGridFilter::Pointer GridFilter = ComputeDistancesGridFilter::New();
  GridFilter->SetInput(image.GetPointer());

  GridFilter->AddInterestValue( 2 );
//  GridFilter->AddInterestValue( 3 );
//  GridOctreeMTFilter->SetBackgroundValue( 0 );
  GridFilter->SetGridDimension(atoi(argv[4]), atoi(argv[4]), atoi(argv[4]));
//  GridOctreeMTFilter->SetComputeBackgroundDistance( false );

  GridFilter->Update();
 
  ImageType::Pointer output = GridFilter->GetOutput();

//  clock.Stop();
//  memoryProbe.Stop();
//  std::cout << "Mean: " << clock.GetMean() << std::endl;
//  std::cout << "Total: " << clock.GetTotal() << std::endl;

//  std::cout<<"\nTask Done!\n";


/*
ImageType::RegionType regionOutput;
ImageType::SizeType regionSizeOutput;
ImageType::PointType originOutput;
ImageType::DirectionType directOutput;
ImageType::SpacingType spOutput;

  // get the region size of the image
  regionOutput = output->GetBufferedRegion();
  regionSizeOutput = regionOutput.GetSize();

  // show region size information
  std::cout << "Output RegionSize = " << regionSizeOutput[0] << ", " << regionSizeOutput[1] << ", " << regionSizeOutput[2] << std::endl;

  // get the spacing along the axis of the image
  spOutput = output->GetSpacing();
  std::cout << "Output Spacing = " << spOutput[0] << ", " << spOutput[1] << ", " << spOutput[2] << std::endl;

  // get the origin in physical coordinates of the image
  originOutput = output->GetOrigin();
  std::cout << "Output Origin = " << originOutput[0] << ", " << originOutput[1] << ", " << originOutput[2] << std::endl;

  // get the direction cosine matrix of the image
  directOutput = output->GetDirection();
  std::cout << "Output Direction = " << std::endl;
  std::cout << directOutput << std::endl;

  std::cout << "Physical extension = " << regionSizeOutput[0] * (float)spOutput[0] << ", " << regionSizeOutput[1] * (float)spOutput[1] << ", " << regionSizeOutput[2] * (float)spOutput[2] << std::endl;
*/
  // Generate test image
  itk::ImageFileWriter<ImageType>::Pointer writer;
  writer = itk::ImageFileWriter<ImageType>::New();
  writer->SetImageIO( itk::NrrdImageIO::New() );
  writer->SetInput( output );
  writer->SetFileName(argv[3]);

  try
  {
    writer->Update();
  }
  catch (itk::ExceptionObject & e)
  {
    std::cerr << "exception in file writer " << std::endl;
    std::cerr << e << std::endl;
    return EXIT_FAILURE;
  }


///////////////////////////////
////COMPARACION DE DISTANCAIAS
//////////////////////////////

  ReaderType::Pointer readerBruteForceImage = ReaderType::New();
  ImageType::Pointer imageBruteForce = ImageType::New();

  readerBruteForceImage->SetFileName(argv[1]);
  readerBruteForceImage->Update();
  imageBruteForce = dynamic_cast<ImageType *>(readerBruteForceImage->GetOutput());

  itk::ImageRegionConstIterator<ImageType> iteratorGroundTrueDistanceMap(imageBruteForce, imageBruteForce->GetLargestPossibleRegion());
  itk::ImageRegionConstIterator<ImageType> iteratorGrid(output, output->GetLargestPossibleRegion());
  using PointType = itk::Point<PixelType, ImageType::ImageDimension>;
  PointType physicalPoint;
  PointType physicalPointGroundTrue;
  ImageType::IndexType pixelIndex;
  ImageType::PixelType gridDistanceValue, groundTrueDistanceValue;
  double diff = 0.0;
  double maxDiff = 0.0;
  double minDiff = std::numeric_limits<double>::max();
  double diffPoints = 0;

   
  using MeasurementVectorType = itk::Vector<float, ImageType::ImageDimension>;
  using SampleType = itk::Statistics::ListSample<MeasurementVectorType>;

  SampleType::Pointer sample = SampleType::New();
  sample->SetMeasurementVectorSize(3);

  MeasurementVectorType mv;

  //lectura de los puntos del grid en itk (el grid en itk es una imagen)
  for (iteratorGrid.GoToBegin(); !iteratorGrid.IsAtEnd(); ++iteratorGrid)
  {
    output->TransformIndexToPhysicalPoint( iteratorGrid.GetIndex(), physicalPoint);

    mv[0] = physicalPoint[0];
    mv[1] = physicalPoint[1];
    mv[2] = physicalPoint[2];
    //se guarda cada punto del grid en el objeto sample
    sample->PushBack(mv);
  }

  using TreeGeneratorType = itk::Statistics::KdTreeGenerator<SampleType>;
  TreeGeneratorType::Pointer treeGenerator = TreeGeneratorType::New();

  // se genera el arbol kd-tree 
  treeGenerator->SetSample(sample);
  treeGenerator->SetBucketSize(16);
  treeGenerator->Update();

  using TreeType = TreeGeneratorType::KdTreeType;
  using NodeType = TreeType::KdTreeNodeType;
  TreeType::Pointer tree = treeGenerator->GetOutput();
  MeasurementVectorType queryPoint, nearPoint;

  using DistanceMetricType = itk::Statistics::EuclideanDistanceMetric<MeasurementVectorType>;
  DistanceMetricType::Pointer distanceMetric = DistanceMetricType::New();
  DistanceMetricType::OriginType originKD(3);
  unsigned int numberOfNeighbors = 1;
  unsigned int i = 0; // counter for loops
  TreeType::InstanceIdentifierVectorType neighbors;

  for (i = 0; i < sample->GetMeasurementVectorSize(); ++i)
  {
    originKD[i] = queryPoint[i];
  }
  distanceMetric->SetOrigin(originKD);

  // se recorre cada punto del ground true (imagen distancia fuerza bruta) y se calcula el punto más cercano en el grid para obtener la distancia en el grid.
  for (iteratorGroundTrueDistanceMap.GoToBegin(); !iteratorGroundTrueDistanceMap.IsAtEnd(); ++iteratorGroundTrueDistanceMap)
  {
    imageBruteForce->TransformIndexToPhysicalPoint( iteratorGroundTrueDistanceMap.GetIndex(), physicalPointGroundTrue);
    groundTrueDistanceValue = iteratorGroundTrueDistanceMap.Get();

    queryPoint[0] = physicalPointGroundTrue[0];
    queryPoint[1] = physicalPointGroundTrue[1];
    queryPoint[2] = physicalPointGroundTrue[2];

   tree->Search(queryPoint, numberOfNeighbors, neighbors);

    nearPoint = tree->GetMeasurementVector(neighbors[0]);

    physicalPoint[0]=nearPoint[0];
    physicalPoint[1]=nearPoint[1];
    physicalPoint[2]=nearPoint[2];

    output->TransformPhysicalPointToIndex(physicalPoint, pixelIndex);

    gridDistanceValue = output->GetPixel(pixelIndex);

    diff += abs(gridDistanceValue - groundTrueDistanceValue);

    if (abs(gridDistanceValue - groundTrueDistanceValue) > maxDiff)
      maxDiff = abs(gridDistanceValue - groundTrueDistanceValue);
	
    if (abs(gridDistanceValue - groundTrueDistanceValue) < minDiff)
      minDiff = abs(gridDistanceValue - groundTrueDistanceValue);

    ++diffPoints;
   }

  std::cout << "#;" << atoi(argv[4]) << ";" << diff / (float)diffPoints << ";" << maxDiff << ";" << minDiff << ";" << gridTime << std::endl;
 

return EXIT_SUCCESS;
}
