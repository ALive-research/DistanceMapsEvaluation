#!/bin/bash

#---COLORS---#
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color


#---Variables---#
build="build"
modules="ITKMaurerDist VTKHausdorff vtk2vtp vtk2vtp ITKBruteForceDist ITKCropImg"


echo "-------------------------------------------------------------------------"
echo "                        Unzip input files"
echo "-------------------------------------------------------------------------"

cd input

for file in $(ls | grep .zip$)
do
    echo -e "[${YELLOW}'$file'${NC}] Uncompressing..."
    unzip $file
done

cd ..

echo "-------------------------------------------------------------------------"
echo "          Compiling Modules for ITK/VTK Distances Test"
echo "-------------------------------------------------------------------------"

for module in $modules
do	     
    echo -e "[${YELLOW}'$module'${NC}] Compiling Module..."
    cd $module
    if [ -d $build ];then
	rm -rf $build
    fi
    mkdir build
    cd build
    cmake ..
    make
    cd ../..
    echo "-------------------------------------------------------------------------"
done
