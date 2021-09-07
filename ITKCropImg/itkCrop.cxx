//Itk
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

  if (argc < 4)
  {
    std::cout << "USAGE: " << argv[0] << " <LiverInput.nrrd> <LiverOutputCrop.nrrd> <padding>" << std::endl;
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
  ImageType::SizeType size = regionLiver.GetSize();

  //*************************************//
  //*       OBJECT BOUNDING BOX         *//
  //*************************************//    
  int x, y, z, Xlim, Ylim, Zlim;
  int Xmin, Xmax;
  int Ymax, Ymin;
  int Zmax, Zmin;
  
  itk::ImageRegionIterator<ImageType> it(imageLiver, regionLiver);
  
  x = 0;
  y = 0;
  z = 0;
  
  Xlim = size[0];
  Ylim = size[1];
  Zlim = size[2];
  
  Xmin = Xlim;
  Ymin = Ylim;
  Zmin = Zlim;
  Xmax = 0;
  Ymax = 0;
  Zmax = 0;
    
  for (it.GoToBegin(); !it.IsAtEnd(); ++it)
  {
    
    if (it.Get() == 1)
    {      
      if (x < Xmin)
	Xmin = x;
      if (x > Xmax)
	Xmax = x;
      
      if (y < Ymin)
	Ymin = y;
      if (y > Ymax)
	Ymax = y;
      
      if (z < Zmin)
	Zmin = z;
      if (z > Zmax)
	Zmax = z;
      
    }
    
    if (x++ == Xlim - 1)
    {
      x = 0;
      if (y++ == Ylim - 1)
      {
	y = 0;
	if (z++ == Zlim - 1)
	{
	  z = 0;
	}
      }
    }
  }

  int padding = atoi(argv[3]);
  
  ImageType::IndexType start;
  ImageType::IndexType end;

  start[0] = Xmin - padding;
  if (start[0] < 0) start[0] = 0;
  
  start[1] = Ymin - padding;
  if (start[1] < 0) start[1] = 0;
  
  start[2] = Zmin - padding;
  if (start[2] < 0) start[2] = 0;

  end[0] = Xmax + padding;
  if (end[0] >= Xlim - 1) end[0] = Xlim - 1;
  
  end[1] = Ymax + padding;
  if (end[1] >= Ylim - 1) end[1] = Ylim - 1;
  
  end[2] = Zmax + padding;
  if (end[2] >= Zlim - 1) end[2] = Zlim - 1;
  
  ImageType::RegionType region;
  region.SetIndex(start);
  region.SetUpperIndex(end);

  using FilterType = itk::RegionOfInterestImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput(imageLiver);
  filter->SetRegionOfInterest(region);
  
  using WriterType = itk::ImageFileWriter<ImageType>;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(argv[2]);
  writer->SetInput(filter->GetOutput());
  try
  {
    writer->Update();
  }
  catch (itk::ExceptionObject & error)
  {
    std::cerr << "Error: " << error << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

