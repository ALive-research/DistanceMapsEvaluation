#!/bin/bash

#---COLORS---#
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[1;36m'
NC='\033[0m' # No Color

#---FILES/PATHS---#
inputPath="input"
outputPath="output"
outputDistPath="outputDist"
output="resample"

#---PARSING ARGUMENTS ---#
bf=0
grid=0

if [ ! $# -eq 2 ]; then
   echo "ERROR: Usage $0 [ALL|BF|GRID] Grid-density"
   exit
fi

if [ $1 = "ALL" ];then	
    bf=1
    grid=1
elif [ $1 = "BF" ];then
    bf=1
elif [ $1 = "GRID" ];then
    grid=1
else
    echo "ERROR: A invalid option found '$1'"
    exit
fi

resampling=$2

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
echo ""

#---OPTIONS PRINT ---#
echo "-------------------------------------------------------------------------"
echo "                Test for Evaluating ITK/VTK Distances"
echo "-------------------------------------------------------------------------"

if [ $bf -eq 1 ];then
    echo "[*] Preprocessing and Brute Force."
else
    echo "[ ] Preprocessing and Brute Force."
fi

if [ $grid -eq 1 ];then
    echo "[*] Grid Distance with ITK/VTK."
else
    echo "[ ] Grid Distance with ITK/VTK."
fi

echo "    Sampling value fixed to '$2'"

echo "-------------------------------------------------------------------------"
#exit

if [ ! -d $outputPath ];then
    mkdir $outputPath
fi

loopGrid="5 10 25 50 100 150"

#---LOOP TESTING---#
files=$(ls $inputPath | grep .nii$)

for file in $files
do
    
    filename="${file%%.*}"
    echo " >  TEST For File: '$file'"     
    outputTestPath=$outputPath"/"$filename

    logFile="$outputTestPath/info.log"
    errorFile="$outputTestPath/errors.log"
    
    if [ $bf -eq 1 ];then
	if [ -d $outputTestPath ];then
	    rm -rf $outputTestPath
	fi
	mkdir $outputTestPath

	echo "" > $logFile
	echo "" > $errorFile
	echo "#FILE=$file" >> $logFile

	#---STEP-1---#
	echo -n -e "    [${YELLOW}STEP 1${NC}] Resampling Image with $resampling,$resampling,$resampling."
	/usr/bin/slicer-modules/ResampleScalarVolume -s $resampling,$resampling,$resampling "$inputPath/$file" "$outputTestPath/$output.nrrd" >> $logFile 2>> $errorFile	
	echo -e "          [${GREEN}Done${NC}]"
	
	#---STEP-2---#
	echo -n -e "    [${YELLOW}STEP 2${NC}] Cropping image." " "
	echo "./ITKCropImg/build/itkCrop \"$outputTestPath/$output.nrrd\" \"$outputTestPath/$output.crop.nrrd\" \"15\""
	./ITKCropImg/build/itkCrop "$outputTestPath/$output.nrrd" "$outputTestPath/$output.crop.nrrd" "15"  >> $logFile 2>> $errorFile
	echo -e "                           [${GREEN}Done${NC}]"
	
	#---STEP-3---#
	echo -n -e "    [${YELLOW}STEP 3${NC}] Building VTK Models from the crop image."    
	/usr/bin/slicer-modules/ModelMaker "--decimate" "0" "--filtertype" "Laplacian" "--smooth" "80" "--generateAll" "$outputTestPath/$output.crop.nrrd" "--name" "$outputTestPath/Model" >> $logFile 2>> $errorFile
	rm "$output.mrml"
	echo -e "    [${GREEN}Done${NC}]"
	
	#---STEP-4---#
	echo -n -e "    [${YELLOW}STEP 4${NC}] Converting Files From VTK Format to VTP."        
	./vtk2vtp/build/vtk2vtp "$outputTestPath/Model_1.vtk" "$outputTestPath/Model_1.vtp" >> $logFile 2>> $errorFile    
	./vtk2vtp/build/vtk2vtp "$outputTestPath/Model_2.vtk" "$outputTestPath/Model_2.vtp" >> $logFile 2>> $errorFile
	echo -e "    [${GREEN}Done${NC}]"
	
	#---STEP-5---#
	echo -n -e "    [${YELLOW}STEP 5${NC}] Computing Brute Force Distance with ITK."    
	./ITKBruteForceDist/build/itkBruteForce "$outputTestPath/$output.crop.nrrd" "$outputTestPath/bruteForceDist.nrrd" >> $logFile 2>> $errorFile
	echo -e "    [${GREEN}Done${NC}]"

	#---STEP-6---#
	echo -n -e "    [${YELLOW}STEP 6${NC}] Computing Maurer Distance with ITK." ""
	./ITKMaurerDist/build/itkMaurer "$outputTestPath/$output.crop.nrrd" "$outputTestPath/maurerDist.nrrd" >> $logFile 2>> $errorFile
	echo -e "        [${GREEN}Done${NC}]"

    fi
    
    if [ $grid -eq 1 ];then
	#---STEP-7---#
	echo -e "    [${YELLOW}STEP 7${NC}] Computing Grids Distances for ITK and VTK."    
	echo "#VTK-GRID-H=GridDensity('$file');Average;Max Diff;Min Diff;HausdorffDist Timing;Grid Timing" >> $logFile
	testId=1
	for gridSamp in $loopGrid
	do
	    printf "             [${YELLOW}STEP 7.$testId${NC}] Testing Grid Density %-8s" $gridSamp
	    ./VTKHausdorff/build/vtkHausdorff "$outputTestPath/bruteForceDist.nrrd" "$outputTestPath/Model_1.vtp" "$outputTestPath/Model_2.vtp" "$outputTestPath/HausdorffDistance.vtp" "$gridSamp" >> $logFile 2>> $errorFile
	    printf "    [${GREEN}Done${NC}] \n"
	    let "testId+=1"
	done
    fi

    echo " >  RESULTS: "
    
    TimeBf=$(cat $logFile | grep "#ITK-BF" | cut -f2 -d"=")
    TimeMaurer=$(cat $logFile | grep "#ITK-MAURER" | cut -f2 -d"=")

    echo -e "    [${YELLOW}#${NC}] Brute Force Time = ${CYAN}$TimeBf${NC}"
    echo -e "    [${YELLOW}#${NC}] Maurer Time      = ${CYAN}$TimeMaurer${NC}"
    echo -e "    [${YELLOW}#${NC}] VTK Grid Test:"
    printf  "        ----------------------------------------------------------\n"
    printf  "        %-15s %-15s %-15s %-15s\n" "Density" "Error Avg" "T Hausdorff(s)" "T Grid(s)"
    printf  "        -----------------------------------------------------------\n"
    for gridSamp in $loopGrid
    do
	error=$(cat $logFile | grep "#VTK-GRID=$gridSamp;" | cut -f2 -d";")
	tHausdorff=$(cat $logFile | grep "#VTK-GRID=$gridSamp;" | cut -f5 -d";")
	tGrid=$(cat $logFile | grep "#VTK-GRID=$gridSamp;" | cut -f6 -d";")
	printf "        ${CYAN}%-15s %-15s %-15s %-15s${NC}\n" $gridSamp $error $tHausdorff $tGrid
    done
    printf  "        -----------------------------------------------------------\n"
    echo -e "-------------------------------------------------------------------------"    
done

echo "Test Done!"
