//ITK
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkTimeProbe.h"
#include "itkImageRegionIterator.h"

//VTK
#include <vtkXMLPolyDataReader.h>
#include <vtkHausdorffDistancePointSetFilter.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkMaskPoints.h>
#include <vtkExecutionTimer.h>
#include <vtkSmartPointer.h>
#include <vtkFieldData.h>
#include <vtkPointData.h>
#include <vtkKdTreePointLocator.h>

//System
#include <iomanip>
#include <iostream>
 


int main(int argc, char *argv[])
{

  if (argc < 6)
  {
    std::cout << "USAGE: " << argv[0] << " <LiverDistInput.nrrd> <Model_1.vtp> <Model_2.vtp> <LiverOutput.vtp> <gridDensity>" << std::endl;
    exit(-1);
  }

  //+++++++++++++++++++++++++++++++++++++++++++//
  //+            [ITK] LOAD IMAGE             +//
  //+++++++++++++++++++++++++++++++++++++++++++//
  using PixelType = float;
  constexpr unsigned int Dimension = 3;  
  using ImageType = itk::Image<PixelType, Dimension>;
  
  //Loading ITK images. First, liver distance Map.
  using ReaderType = itk::ImageFileReader<ImageType>;
  ReaderType::Pointer readerLiver = ReaderType::New();
  const char *filenameLiver = argv[1];
  readerLiver->SetFileName(filenameLiver);
  readerLiver->Update();
  
  ImageType::Pointer imageLiver = readerLiver->GetOutput();  
  ImageType::RegionType regionLiver  = imageLiver->GetLargestPossibleRegion();  
  ImageType::SizeType size = regionLiver.GetSize();
    
  const ImageType::PointType & origin = imageLiver->GetOrigin();
  const ImageType::SpacingType & sp   = imageLiver->GetSpacing();

  //Calculating Grid Size in physical Space
  float physicSize[3] = {size[0] * (float)sp[0], size[1] * (float)sp[1], size[2] * (float)sp[2]};
  
  ImageType::DirectionType direct = imageLiver->GetDirection();
  
  itk::TimeProbe distClock, gridClock;
  double tGrid, tDist;
      
  //+++++++++++++++++++++++++++++++++++++++++++//
  //+  [VTK] LOAD IMAGE & GRID CONSTURCTION   +//
  //+++++++++++++++++++++++++++++++++++++++++++//  
  //Loading liver model into polydata
  vtkNew<vtkXMLPolyDataReader> modelReader;
  modelReader->SetFileName(argv[2]);
  modelReader->Update();
  vtkPolyData *modelPolyData = modelReader->GetOutput();   
  
  //Loading tumor model into polydata
  vtkNew<vtkXMLPolyDataReader> targetReader;
  targetReader->SetFileName(argv[3]);
  targetReader->Update();
  vtkPolyData *targetPolyData = targetReader->GetOutput();
  
  gridClock.Start();
  //Grid construction
  int gridPoints;
  double bounds[6];  
  double gridPoint[3];

  //Calculating Grid Bounds. For ecah axis x,y,z:
  //  [*] Get Image origin from ITK (Physical space)
  //  [*] Get Direction matrix and calculate the bounds
  gridPoint[0] = origin[0] + direct[0][0] * physicSize[0];
  gridPoint[1] = origin[1] + direct[1][1] * physicSize[1];
  gridPoint[2] = origin[2] + direct[2][2] * physicSize[2];    
  for (int i = 0; i < 3; i++)
    if (origin[i] > gridPoint[i]) {
      bounds[i*2] = gridPoint[i];
      bounds[i*2 + 1] = origin[i];
    } else {
      bounds[i*2] = origin[i];
      bounds[i*2 + 1] = gridPoint[i];
    }
  
  vtkNew<vtkPoints> points;
  unsigned int sampGrid = atoi(argv[5]);
  
  //Grid Bounds
  double Xmin = bounds[0];
  double Xmax = bounds[1];
  double Ymin = bounds[2];
  double Ymax = bounds[3];
  double Zmin = bounds[4];
  double Zmax = bounds[5];  
  
  //Calculating Grid Chunk Size
  double Xchunk = (Xmax - Xmin) / sampGrid;
  double Ychunk = (Ymax - Ymin) / sampGrid;
  double Zchunk = (Zmax - Zmin) / sampGrid;
    
  for(unsigned int x = 0; x <= sampGrid; x++)
  {    
    for(unsigned int y = 0; y <= sampGrid; y++)
    {
      for(unsigned int z = 0; z <= sampGrid; z++)
	{
	  double X = Xmin + Xchunk*x;
	  double Y = Ymin + Ychunk*y;
	  double Z = Zmin + Zchunk*z;
	  //Controll that all points in the grid are into the limits
	  if (X <= Xmin + 1)
	    X = Xmin + 1;
	  if (X >= Xmax - 1)
	    X = Xmax - 1;
	  if (Y <= Ymin + 1)
	    Y = Ymin + 1;
	  if (Y >= Ymax - 1)
	    Y = Ymax - 1;	  
	  if (Z <= Zmin + 1)
	    Z = Zmin + 1;
	  if (Z >= Zmax - 1)
	    Z = Zmax - 1;
	  points->InsertNextPoint(X, Y, Z);
	}
    }
  }
  
  vtkNew<vtkPolyData> pointsPolyData;
  pointsPolyData->SetPoints(points);
    
  vtkNew<vtkMaskPoints> maskPoints;
  maskPoints->SetInputData(pointsPolyData);
  maskPoints->SetOnRatio(1);
  maskPoints->GenerateVerticesOn();
  maskPoints->SingleVertexPerCellOn();
  maskPoints->Update();
  
  gridClock.Stop();
  tGrid = gridClock.GetTotal();

  //+++++++++++++++++++++++++++++++++++++++++++//
  //+         [VTK] HAUSDORFF DISTANCE        +//
  //+++++++++++++++++++++++++++++++++++++++++++//
  distClock.Start();
  
  vtkNew<vtkHausdorffDistancePointSetFilter> filter;
  filter->SetInputData(0, maskPoints->GetOutput());
  filter->SetInputData(1, targetPolyData);
  filter->SetTargetDistanceMethod( 0 );
  filter->Update();

  distClock.Stop();
  tDist = distClock.GetTotal();
  
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
  //+    [ITK/VTK] COMPARE DISTANCE FROM ITK (IMAGE) TO VTK +//
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++//  
  vtkNew<vtkKdTreePointLocator> kDTree;
  kDTree->SetDataSet(pointsPolyData);
  kDTree->BuildLocator();
  ImageType::PointType physPoint;
  double physPointKDTree[3];
  double closestPoint[3];					
  vtkPointSet *HausdorffOutput = static_cast<vtkPointSet*>(filter->GetOutput(0));
  size_t nPoints = HausdorffOutput->GetNumberOfPoints();
  
  const int loopBreak = 10;
  int itn = 0;
  double avgTot = 0;
  double avg;
  size_t totPoints = 0;
  double maxDiff = 0.0;
  double minDiff = 10000000.0;

  itk::ImageRegionConstIterator<ImageType> it(imageLiver, regionLiver);
  for (it.GoToBegin(); !it.IsAtEnd(); ++it)
  {
    imageLiver->TransformIndexToPhysicalPoint(it.GetIndex(), physPoint);
    physPointKDTree[0] = physPoint[0];
    physPointKDTree[1] = physPoint[1];
    physPointKDTree[2] = physPoint[2];
    vtkIdType iD = kDTree->FindClosestPoint(physPointKDTree);
    kDTree->GetDataSet()->GetPoint(iD, closestPoint);
    double pixelValue = it.Value();
    double value = HausdorffOutput->GetPointData()->GetArray("Distance")->GetComponent(iD, 0);
    
    totPoints++;
    avg = abs(value - pixelValue);
    avgTot += avg;
    
    if (avg > maxDiff)
      maxDiff = avg;
    
    if (avg < minDiff)
      minDiff = avg;
    
    //std::cout << "[REFER]ImgPoint(" << physPointKDTree[0] <<  "," << physPointKDTree[1] << "," << physPointKDTree[2] << ")="
    //	      << pixelValue << " vs " << "[TARGET] Grid Closest Point(" << closestPoint[0] << "," << closestPoint[1] << ","
    //	      << closestPoint[2] << ")=" << value << " => Difference=" << abs(value - pixelValue) << std::endl;
    //if (++itn == loopBreak)
    //break;    
  }

  cout.precision(3);
  std::cout << "#VTK-GRID=" << sampGrid << ";" << avgTot / totPoints << ";" << maxDiff << ";" << minDiff
	    << ";"  << tDist << ";" << tGrid << std::endl;
  
  //+++++++++++++++++++++++++++++++++++++++++++//
  //+      [VTK] WRITE HAUSDORFF DISTANCE     +//
  //+++++++++++++++++++++++++++++++++++++++++++//   
  vtkNew<vtkXMLPolyDataWriter> polyDataWriter;
  polyDataWriter->SetInputConnection(filter->GetOutputPort(0));
  polyDataWriter->SetFileName(argv[4]);
  polyDataWriter->Update();
  
}
