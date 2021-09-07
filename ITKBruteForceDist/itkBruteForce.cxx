//ITK
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkTimeProbe.h"
#include "itkObjectToObjectDistanceMap.h"
#include "itkNrrdImageIO.h"


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
  
  //Duplicate the image for the Grid    
  itk::TimeProbe clock;
  double timing;
  
  int gridPoints;
  
  clock.Start();
  using itkObjectToObjectDistanceMapFilter = itk::itkObjectToObjectDistanceMap<ImageType>;
  itkObjectToObjectDistanceMapFilter::Pointer BruteForceDistanceMapFilter = itkObjectToObjectDistanceMapFilter::New();
  BruteForceDistanceMapFilter->SetInput(imageLiver.GetPointer());  
  BruteForceDistanceMapFilter->AddInterestValue( 2 );
  BruteForceDistanceMapFilter->SetBackgroundValue( 0 );
  BruteForceDistanceMapFilter->SetComputeBackgroundDistance(true);
  
  // read image in .nrrd
  BruteForceDistanceMapFilter->Update();  
  ImageType::Pointer bruteOutput = BruteForceDistanceMapFilter->GetOutput();

  clock.Stop();
  
  itk::ImageFileWriter<ImageType>::Pointer writerLiver;
  writerLiver = itk::ImageFileWriter<ImageType>::New();
  writerLiver->SetImageIO( itk::NrrdImageIO::New() );
  writerLiver->SetInput( bruteOutput );
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

  std::cout << "#ITK-BF=" << clock.GetTotal() << "(s)" << std::endl;
  
}
