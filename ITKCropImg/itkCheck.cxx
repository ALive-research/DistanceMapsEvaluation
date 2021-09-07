//ITK
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIterator.h"

//System
#include <iomanip>
#include <iostream>


int main(int argc, char *argv[])
{

  if (argc < 3)
  {
    std::cout << "USAGE: " << argv[0] << " <LiverInputLarge.nrrd> <LiverInputCrop.vtp> " << std::endl;
    exit(-1);
  }

  //+++++++++++++++++++++++++++++++++++++++++++//
  //+            [ITK] LOAD IMAGE             +//
  //+++++++++++++++++++++++++++++++++++++++++++//
  using PixelType = float;
  constexpr unsigned int Dimension = 3;  
  using ImageType = itk::Image<PixelType, Dimension>;
  
  //LOAD IMAGE 1
  using ReaderType = itk::ImageFileReader<ImageType>;
  ReaderType::Pointer readerLiverLarge = ReaderType::New();
  const char *filenameLiverLarge = argv[1];
  readerLiverLarge->SetFileName(filenameLiverLarge);
  readerLiverLarge->Update();

  //LOAD IMAGE 2
  using ReaderType = itk::ImageFileReader<ImageType>;
  ReaderType::Pointer readerLiverCrop = ReaderType::New();
  const char *filenameLiverCrop = argv[2];
  readerLiverCrop->SetFileName(filenameLiverCrop);
  readerLiverCrop->Update();

  
  ImageType::Pointer imageLiverLarge = readerLiverLarge->GetOutput();
  ImageType::Pointer imageLiverCrop  = readerLiverCrop->GetOutput();
  
  ImageType::RegionType regionLiverLarge = imageLiverLarge->GetLargestPossibleRegion();
  ImageType::SizeType sizeLarge = regionLiverLarge.GetSize();

  ImageType::RegionType regionLiverCrop = imageLiverCrop->GetLargestPossibleRegion();
  ImageType::SizeType sizeCrop = regionLiverCrop.GetSize();

  std::cout << "Size Large=[" << sizeLarge[0] << "," << sizeLarge[1] << "," << sizeLarge[2] << "] vs Size Crop=["
  	    << sizeCrop[0] << "," << sizeCrop[1] << "," << sizeCrop[2] << "]" << std::endl;
  
  const ImageType::SpacingType & spLarge = imageLiverLarge->GetSpacing();
  const ImageType::SpacingType & spCrop  = imageLiverCrop->GetSpacing();

  std::cout << "Spacing Large=[" << spLarge[0] << "," << spLarge[1] << "," << spLarge[2] << "] vs Spacing Crop=["
  	    << spCrop[0] << "," << spCrop[1] << "," << spCrop[2] << "]" << std::endl;


  const ImageType::PointType & originLarge = imageLiverLarge->GetOrigin();
  const ImageType::PointType & originCrop  = imageLiverCrop->GetOrigin();

  std::cout << "Origin Large=[" << originLarge[0] << "," << originLarge[1] << "," << originLarge[2] << "] vs Origin Crop=["
  	    << originCrop[0] << "," << originCrop[1] << "," << originCrop[2] << "]" << std::endl;

  const ImageType::DirectionType & directLarge = imageLiverLarge->GetDirection();
  const ImageType::DirectionType & directCrop  = imageLiverCrop->GetDirection();

  std::cout << "Direction Large:" << std::endl;
  std::cout << directLarge << std::endl;
  std::cout << "Direction Crop:" << std::endl;
  std::cout << directCrop << std::endl;
  
  return EXIT_SUCCESS;
  
}

