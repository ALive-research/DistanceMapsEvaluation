#!/usr/bin/env bash

# Absolute path to this script. /home/user/bin/foo.sh
SCRIPT=$(readlink -f $0)
# Absolute path this script is in. /home/user/bin
SCRIPTPATH=`dirname $SCRIPT`
# Load environment variables
. ${SCRIPTPATH}/environment.sh

# Select output file
if [ $# -eq 0 ]; then
   LIVER_OUTPUT_FILE="/tmp/liver.csv"
   TUMOR_OUTPUT_FILE="/tmp/tumor.csv"
else
    LIVER_OUTPUT_FILE="$1/liver.csv"
    TUMOR_OUTPUT_FILE="$1/tumor.csv"
fi


# Build the CSV file header
CSV_HEADER="Item"
CSV_HEADER+=",Size[0],Size[1],Size[2]"            # Image Size
CSV_HEADER+=",Spacing[0],Spacing[1],Spacing[2]"   # Image Spacing
CSV_HEADER+=",BBIndex[0],BBIndex[1],BBIndex[2]"   # Index to Bounding Box
CSV_HEADER+=",BBSize[0],BBSize[1],BBSize[2]"      # Bounding Box Size
CSV_HEADER+=",Maurer_Time (ms)"                   # Maurer Time
CSV_HEADER+=",Downsample_Time (ms)"               # Downsample Time
#CSV_HEADER+="BF_Time"                            # Brute-force Time

# Resampling values
RESAMPLING="25 50 100 200"
CSV_HEADER_LIVER_RESAMPLING=""
CSV_HEADER_TUMOR_RESAMPLING=""
for i in ${RESAMPLING}
do
    CSV_HEADER_LIVER_RESAMPLING+=",Maurer_liver_resampling_t[$i]" # Time for resampling process (liver at [xxx] resampling)
    CSV_HEADER_TUMOR_RESAMPLING+=",Maurer_tumor_resampling_t[$i]" # Time for resampling process (tumor at [xxx] resampling)
done
CSV_LIVER_HEADER=${CSV_HEADER}${CSV_HEADER_LIVER_RESAMPLING}
CSV_TUMOR_HEADER=${CSV_HEADER}${CSV_HEADER_TUMOR_RESAMPLING}

# Clear the CSV files and print the header
echo ${CSV_LIVER_HEADER} > ${LIVER_OUTPUT_FILE}
echo ${CSV_TUMOR_HEADER} > ${TUMOR_OUTPUT_FILE}

# Process Liver Data
items=$(ls ${Dataset_DIR}/*liver*.sha256 | wc -l)
declare -i index=1
for i in $(ls ${Dataset_DIR}/*liver*.sha256)
do
    echo "[${index}/${items}] Processing $(basename ${i})..."

    # File name definitions
    echo -e "\t Downloading liver image"
    liver=$(data $(basename ${i}))
    liver_cropped=${liver%.*}_cropped.nrrd
    liver_maurer=${liver%.*}_maurer.nrrd
    liver_bf=${liver%.*}_bf.nrrd
    #-------------------------------------------
    echo -e "\t Downloading tumor image"
    tumor=$(data $(basename ${i/liver/tumor}))
    tumor_cropped=${tumor%.*}_cropped.nrrd
    tumor_cropped_maurer=${tumor%.*}_cropped_maurer.nrrd
    tumor_bf=${tumor%.*}_bf.nrrd

    echo -e "\t Getting image geometry"

    # Obtain spacing and size of the image
    # NOTE: Threre should be a way to reduce these to a single line, or at least to a single awk call
    spacing_x=$(head $liver -n 100 | awk '/space directions:/' | awk -F':' '{print $2}'| awk '{print $1}' | awk -F'[][()]' '{print $2}'| awk -F',' '{print $1<0?-$1:$1}')
    spacing_y=$(head $liver -n 100 | awk '/space directions:/' | awk -F':' '{print $2}'| awk '{print $2}' | awk -F'[][()]' '{print $2}'| awk -F',' '{print $2<0?-$2:$2}')
    spacing_z=$(head $liver -n 100 | awk '/space directions:/' | awk -F':' '{print $2}'| awk '{print $3}' | awk -F'[][()]' '{print $2}'| awk -F',' '{print $3<0?-$3:$3}')
    size_x=$(head $liver -n 100 | awk '/sizes/' | awk -F':' '{print $2}' | awk '{print $1}')
    size_y=$(head $liver -n 100 | awk '/sizes/' | awk -F':' '{print $2}' | awk '{print $2}')
    size_z=$(head $liver -n 100 | awk '/sizes/' | awk -F':' '{print $2}' | awk '{print $3}')

    echo -e "\t Computing bounding box for liver image"

    # Compute bounding box and crop the liver image
    bounding_box=$(${BOUNDINGBOX_OPERATOR} -i $liver -o ${liver%.*}_cropped.nrrd)
    # NOTE: Threre should be a way to reduce these to a single line, or at least to a single awk call
    offset_x=$(echo ${bounding_box} | awk -F': ' '{print $4}' | awk -F'[][]' '{print $2}'| awk -F', ' '{print $1}')
    offset_y=$(echo ${bounding_box} | awk -F': ' '{print $4}' | awk -F'[][]' '{print $2}'| awk -F', ' '{print $2}')
    offset_z=$(echo ${bounding_box} | awk -F': ' '{print $4}' | awk -F'[][]' '{print $2}'| awk -F', ' '{print $3}')
    bb_size_x=$(echo ${bounding_box} | awk -F': ' '{print $5}' | awk -F'[][]' '{print $2}'| awk -F', ' '{print $1}')
    bb_size_y=$(echo ${bounding_box} | awk -F': ' '{print $5}' | awk -F'[][]' '{print $2}'| awk -F', ' '{print $2}')
    bb_size_z=$(echo ${bounding_box} | awk -F': ' '{print $5}' | awk -F'[][]' '{print $2}'| awk -F', ' '{print $3}')

    # Crop the tumor image
    echo -e "\t Cropping tumor image"
    ${CROP_OPERATOR} -i $tumor -x $offset_x -y $offset_y -z $offset_z -u $bb_size_x -v $bb_size_y -w $bb_size_z -o $tumor_cropped

    # Apply Maurer
    echo -e "\t Computing Maure distance (liver)"
    liver_maurer_t=$(${MAURER_OPERATOR} -i $liver -o $liver_maurer | awk -F':' '{print $2}')

    echo -e "\t Computing Maure distance (tumor)"
    tumor_maurer_t=$(${MAURER_OPERATOR} -i $tumor_cropped -o $tumor_cropped_maurer | awk -F':' '{print $2}')

    # Apply Brute-force
    # liver_bf_t=$(${DISTANCE_OPERATOR} -i $liver -o $liver_bf| awk -F':' '{print $2}')
    # tumor_bf_t=$(${DISTANCE_OPERATOR} -i $tumor -o $tumor_bf| awk -F':' '{print $2}')

    # Resampling
    LIVER_CSV_ROW_RESAMPLE=""
    TUMOR_CSV_ROW_RESAMPLE=""
    for i in $RESAMPLING
    do
        echo -e "\t Downsampling maurer distance (liver -- ${i})"
        liver_maurer_downsampled=${liver%.*}_maurer_downsampled_${i}.nrrd
        t=$(${RESAMPLE_OPERATOR} -i ${liver_maurer} -o ${liver_maurer_downsampled} -d 2 -l 1 -x $i -y $i -z $i)
        LIVER_CSV_ROW_RESAMPLE+=",${t}"

        echo -e "\t Downsampling maurer distance (tumor -- ${i})"
        tumor_cropped_maurer_downsampled=${tumor%.*}_maurer_downsampled_${i}.nrrd
        t=$(${RESAMPLE_OPERATOR} -i ${tumor_cropped_maurer} -o ${tumor_cropped_maurer_downsampled} -d 2 -l 1 -x $i -y $i -z $i)
        TUMOR_CSV_ROW_RESAMPLE+=",${t}"

        echo -e "\t Upsampling maurer distance (liver -- ${i})"
        liver_maurer_upsampled=${liver%.*}_maurer_upsampled_${i}.nrrd
        ${RESAMPLE_OPERATOR} -i ${liver_maurer_downsampled} -o ${liver_maurer_upsampled} -d 2 -l 1 -x $size_x -y $size_y -z $size_z > /dev/null

        echo -e "\t Upsampling maurer distance (tumor -- ${i})"
        tumor_cropped_maurer_upsampled=${tumor%.*}_maurer_upsampled_${i}.nrrd
        ${RESAMPLE_OPERATOR} -i ${tumor_cropped_maurer_downsampled} -o ${tumor_cropped_maurer_upsampled} -d 2 -l 1 -x $bb_size_x -y $bb_size_y -z $bb_size_z > /dev/null

        echo -e "\t Comparing interpolated downsampled and original distance maps (liver-- ${i})"
        liver_maurer_difference=${liver%.*}_maurer_difference_${i}.nrrd
        ${COMPARE_OPERATOR} -a $liver_maurer -b $liver_maurer_upsampled -k $liver -l 1 -d $liver_maurer_difference -M 10000 -m 10000 -s 10000 -e 10000

        echo -e "\t Comparing interpolated downsampled and original distance maps (tumor-- ${i})"
        tumor_cropped_maurer_difference=${tumor%.*}_maurer_difference_${i}.nrrd
        ${COMPARE_OPERATOR} -a $tumor_cropped_maurer -b $tumor_cropped_maurer_upsampled -k $tumor_cropped -l 1 -o -d $tumor_cropped_maurer_difference -M 10000 -m 10000 -s 10000 -e 10000
    done

    LIVER_CSV_ROW="${liver%.*}"
    LIVER_CSV_ROW+=",${size_x},${size_y},${size_z}"
    LIVER_CSV_ROW+=",${spacing_x},${spacing_y},${spacing_z}"
    LIVER_CSV_ROW+=",${offset_x},${offset_y},${offset_z}"
    LIVER_CSV_ROW+=",${bb_size_x},${bb_size_y},${bb_size_z}"
    LIVER_CSV_ROW+=",${liver_maurer_t}"
    LIVER_CSV_ROW+=${LIVER_CSV_ROW_RESAMPLE}

    TUMOR_CSV_ROW="${tumor%.*}"
    TUMOR_CSV_ROW+=",${size_x},${size_y},${size_z}"
    TUMOR_CSV_ROW+=",${spacing_x},${spacing_y},${spacing_z}"
    TUMOR_CSV_ROW+=",${offset_x},${offset_y},${offset_z}"
    TUMOR_CSV_ROW+=",${bb_size_x},${bb_size_y},${bb_size_z}"
    LIVER_CSV_ROW+=",${tumor_maurer_t}"
    TUMOR_CSV_ROW+=${TUMOR_CSV_ROW_RESAMPLE}

    echo ${LIVER_CSV_ROW} >> ${LIVER_OUTPUT_FILE}
    echo ${TUMOR_CSV_ROW} >> ${TUMOR_OUTPUT_FILE}

    index+=1
done
