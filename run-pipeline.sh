#!/bin/bash

resampling=0.5

for file in $(ls input | grep ".zip$" );
do

    unzip "input/$file" -d "input/"

    in=$(echo $file | cut -f1 -d".")".nii"
    
    #---STEP-1---#
    /usr/bin/slicer-modules/ResampleScalarVolume -s $resampling,$resampling,$resampling "input/$in" "output/resample.nrrd"
    
    #---STEP-2---#
    /usr/bin/slicer-modules/ModelMaker "--decimate" "0" "--filtertype" "Laplacian" "--smooth" "80" "--generateAll" "output/resample.nrrd" "--name" "output/Model"
    rm "resample.mrml"
    
    #---STEP-3---#
    ./vtk2vtp/build/vtk2vtp "output/Model_1.vtk" "output/Model_1.vtp"
    ./vtk2vtp/build/vtk2vtp "output/Model_2.vtk" "output/Model_2.vtp"
    
    #---STEP-6---#
    ./ITKMaurerDist/build/itkMaurer "output/resample.nrrd" "output/maurerDist.nrrd"

    #---STEP-7---#
    ./VTKHausdorff/build/vtkHausdorffDist output/resample.nrrd output/maurerDist.nrrd output/Model_1.vtp output/Model_2.vtp ./output/hausdorffDistance.vtp 50

done

