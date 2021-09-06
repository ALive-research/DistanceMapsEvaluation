//ITK
#include "itkImageFileReader.h"
#include "itkTimeProbe.h"


//VTK
#include <vtkXMLPolyDataReader.h>
#include <vtkHausdorffDistancePointSetFilter.h>
#include <vtkMaskPoints.h>
#include <vtkPointData.h>
#include <vtkXMLPolyDataWriter.h>

//System
 


int main(int argc, char *argv[])
{

  if (argc < 6)
  {
    std::cout << "USAGE: " << argv[0] << " <LiverInput.nrrd> <LiverDistInput.nrrd> <Model_1.vtp> <Model_2.vtp> <LiverOutput.vtp>" << std::endl;
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
  ImageType::RegionType regionLiver  = imageLiver->GetLargestPossibleRegion();  
  ImageType::SizeType size = regionLiver.GetSize();

  ReaderType::Pointer readerLiverDist = ReaderType::New();
  const char *filenameLiverDist = argv[2];
  readerLiverDist->SetFileName(filenameLiverDist);
  readerLiverDist->Update();
  ImageType::Pointer imageLiverDist = readerLiverDist->GetOutput();  
  ImageType::RegionType regionLiverDist  = imageLiverDist->GetLargestPossibleRegion();  
  ImageType::SizeType sizeDist = regionLiverDist.GetSize();
  
  
  itk::TimeProbe distClock, gridClock;
  double tGrid, tDist;
      
  //+++++++++++++++++++++++++++++++++++++++++++//
  //+  [VTK] LOAD IMAGE & GRID CONSTURCTION   +//
  //+++++++++++++++++++++++++++++++++++++++++++//  
  //POLYDATA LOAD MODEL (liver)
  vtkNew<vtkXMLPolyDataReader> modelReader;
  modelReader->SetFileName(argv[3]);
  modelReader->Update();
  vtkPolyData *modelPolyData = modelReader->GetOutput();   
  
  //POLYDATA LOAD TARGET (tumor)
  vtkNew<vtkXMLPolyDataReader> targetReader;
  targetReader->SetFileName(argv[4]);
  targetReader->Update();
  vtkPolyData *targetPolyData = targetReader->GetOutput();
  //std::cout << "*VTK Computing Grid Hausdorff Distance..." << std::endl;

  //+++++++++++++++++++++++++++++++++++++++++++//
  //+         [VTK] HAUSDORFF DISTANCE        +//
  //+++++++++++++++++++++++++++++++++++++++++++//
  distClock.Start();
  
  vtkNew<vtkHausdorffDistancePointSetFilter> filter;
  filter->SetInputData(0, modelPolyData);
  filter->SetInputData(1, targetPolyData);
  filter->SetTargetDistanceMethod( 0 );  
  filter->Update();

  distClock.Stop();
  tDist = distClock.GetTotal();

  double *b = modelPolyData->GetBounds();
  std::cout << "Xmin = " << b[0] << "," << std::endl;
  std::cout << "Xmax = " << b[1] << "," << std::endl;
  std::cout << "Ymin = " << b[2] << "," << std::endl;
  std::cout << "Ymax = " << b[3] << "," << std::endl;
  std::cout << "Zmin = " << b[4] << "," << std::endl;
  std::cout << "Zmax = " << b[5] << "," << std::endl;

  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
  //+    [ITK/VTK] COMPARE DISTANCE FROM ITK (IMAGE) TO VTK +//
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
  vtkPointSet *HausdorffOutput = static_cast<vtkPointSet*>(filter->GetOutput(0));
  vtkPoints *polyPoints = modelPolyData->GetPoints();
  double avgTot = 0;
  double avg;
  size_t tpoints = 0;
  double minDistance = 10000000;
  double maxError = 0;

  for (vtkIdType i = 0; i < polyPoints->GetNumberOfPoints(); i++)
  {
    double p[3];
    polyPoints->GetPoint(i, p);
    double value = HausdorffOutput->GetPointData()->GetArray("Distance")->GetComponent(i, 0);

    ImageType::IndexType pixelIndex;    
    using PointType = itk::Point<float, ImageType::ImageDimension>;
    PointType point;    
    point[0] = p[0]; // x coordinate
    point[1] = p[1]; // y coordinate
    point[2] = p[2]; // z coordinate
    imageLiverDist->TransformPhysicalPointToIndex(point, pixelIndex);    
    ImageType::PixelType pixelValue = imageLiverDist->GetPixel(pixelIndex);

    if (pixelValue != -1) {
      avg = abs(value - pixelValue);
      avgTot += avg;
      tpoints++;
     }

    if (avg > maxError) {
      std::cout << "Point Coord=(" << p[0] << "," << p[1] << "," << p[2] << ") => VTK-Distance = " <<
	value << " vs ITK-Distance = " << pixelValue << " => New Max Error = " << avg << std::endl;
      maxError = avg;
    }
    
    if (value < minDistance)
      minDistance = value;      
  }

  std::cout << "[*] Points Number = " << tpoints << std::endl;
  std::cout << "[*] Error Average = " << avgTot / tpoints << std::endl;
  std::cout << "[*] Max Error     = " << maxError << std::endl;
  std::cout << "[*] Min Distance  = " << minDistance << std::endl;

  //+++++++++++++++++++++++++++++++++++++++++++//
  //+      [VTK] WRITE HAUSDORFF DISTANCE     +//
  //+++++++++++++++++++++++++++++++++++++++++++//  
  
  vtkNew<vtkXMLPolyDataWriter> polyDataWriter;
  polyDataWriter->SetInputConnection(filter->GetOutputPort(0));
  polyDataWriter->SetFileName(argv[5]); //"VTK-HausdorffDist.vtp"
  polyDataWriter->Update();

}
