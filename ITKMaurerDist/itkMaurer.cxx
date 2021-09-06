//ITK
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkTimeProbe.h"
#include "itkNrrdImageIO.h"
#include "itkSignedMaurerDistanceMapImageFilter.h"

//System
//#include <iomanip>
//#include <iostream>


int main(int argc, char *argv[])
{
  
  if (argc < 3)
  {
    std::cout << "USAGE: " << argv[0] << " <LiverInput.nrrd> <DistanceOutput.nrrd> " << std::endl;
    exit(-1);
  }

  //+++++++++++++++++++++++++++++++++++++++++++//
  //+            [ITK] LOAD IMAGE             +//
  //+++++++++++++++++++++++++++++++++++++++++++//
  using PixelType = float;
  constexpr unsigned int Dimension = 3;  
  using ImageType = itk::Image<PixelType, Dimension>;
  
  //LOAD IMAGE
  using ReaderType = itk::ImageFileReader<ImageType>;
  ReaderType::Pointer readerLiver = ReaderType::New();
  const char *filenameLiver = argv[1];
  readerLiver->SetFileName(filenameLiver);
  readerLiver->Update();
  
  ImageType::Pointer imageLiver = readerLiver->GetOutput();
  ImageType::RegionType regionLiver = imageLiver->GetLargestPossibleRegion();  
  //Duplicate the image for the Grid    
  itk::TimeProbe clock;
  double timing;
  
  int gridPoints;

  clock.Start();
  
  using SignedMaurerDistanceMapImageFilterType = itk::SignedMaurerDistanceMapImageFilter<ImageType, ImageType>;
  SignedMaurerDistanceMapImageFilterType::Pointer distanceMapImageFilter = SignedMaurerDistanceMapImageFilterType::New();
  
  distanceMapImageFilter->SetBackgroundValue(2);
  distanceMapImageFilter->SetUseImageSpacing(true);
  distanceMapImageFilter->SetInsideIsPositive(true);
  distanceMapImageFilter->SetInput(imageLiver);
  
  ImageType::Pointer maurerOutput = distanceMapImageFilter->GetOutput();
  
  maurerOutput->SetRegions(regionLiver);
  maurerOutput->SetSpacing(imageLiver->GetSpacing());
  maurerOutput->SetOrigin(imageLiver->GetOrigin());
  maurerOutput->Allocate();
  maurerOutput->Update();

  itk::ImageRegionConstIterator<ImageType> itA(imageLiver, regionLiver);
  itk::ImageRegionIterator<ImageType> outputImageIterator(maurerOutput, maurerOutput->GetLargestPossibleRegion());
  for (itA.GoToBegin(),outputImageIterator.GoToBegin(); !itA.IsAtEnd(); ++itA,++outputImageIterator)
  {
     float voxelValueA = (int)itA.Get();
    if (voxelValueA == 0) {
      outputImageIterator.Set(-1);
    } 
  }
  clock.Stop();  
  
  itk::ImageFileWriter<ImageType>::Pointer writerLiver;
  writerLiver = itk::ImageFileWriter<ImageType>::New();
  writerLiver->SetImageIO( itk::NrrdImageIO::New() );
  writerLiver->SetInput( maurerOutput );
  writerLiver->SetFileName(argv[2]); //"ITK-MauererDist.nrrd"

  try
  {
    writerLiver->Update();
  }
  catch (itk::ExceptionObject & e)
  {
    std::cerr << "exception in file writer " << std::endl;
    std::cerr << e << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "#ITK-MAURER=" << clock.GetTotal() << "(s)" << std::endl;
  
}
