#!/usr/bin/env bash

# This is a script used to convert datasets from the medical segmentatino decathlon
# to new datasets where the anatomical structures and tumors are separated in
# different datasets.

# Absolute path to this script. /home/user/bin/foo.sh
SCRIPT=$(readlink -f $0)
# Absolute path this script is in. /home/user/bin
SCRIPTPATH=`dirname $SCRIPT`
# Load environment variables
. ${SCRIPTPATH}/environment.sh

LIVER_DATASET=$(find $2 -type f)
VESSELS_DATASET=$(find $3 -type f)
ALL_DATASET=$(find $2 $3 -type f)

set -eux

# Process parenchyma
declare -i livernr=0
for i in ${LIVER_DATASET}; do
    echo "Processing ${i}"
    ${LABELEXTRACT_OPERATOR} -l 1 -L 1 -i ${i} -o ${1}/liver_$(printf %03d $livernr).nrrd
    livernr+=1
done

# Process vessels
declare -i vesselnr=0
for i in ${VESSELS_DATASET}; do
    echo "Processing ${i}"
    ${LABELEXTRACT_OPERATOR} -l 1 -L 1 -i ${i} -o ${1}/vessels_$(printf %03d $vesselnr).nrrd
    vesselnr+=1
done

# Process tumors
declare -i tumornr=0
for i in ${ALL_DATASET}; do
    echo "Processing ${i}"
    ${LABELEXTRACT_OPERATOR} -l 2 -L 1 -i ${i} -o ${1}/tumor_$(printf %03d $tumornr).nrrd
    tumornr+=1
done
