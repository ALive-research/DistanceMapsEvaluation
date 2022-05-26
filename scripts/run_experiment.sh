#!/usr/bin/env bash

# Absolute path to this script. /home/user/bin/foo.sh
SCRIPT=$(readlink -f $0)
# Absolute path this script is in. /home/user/bin
SCRIPTPATH=`dirname $SCRIPT`
# Load environment variables
. ${SCRIPTPATH}/environment.sh

echo "Item,Size[0],Size[1],Size[2],Spacing[0],Spacing[1],Spacing[2]" > /tmp/out.csv

# Process the data
for i in $(ls ${Dataset_DIR}/*.sha256)
do
    spacing_x=$(head $(data $(basename ${i})) -n 1000 | awk '/space directions:/' | awk -F':' '{print $2}'| awk '{print $1}' | awk -F'[][()]' '{print $2}'| awk -F',' '{print $1<0?-$1:$1}')
    spacing_y=$(head $(data $(basename ${i})) -n 1000 | awk '/space directions:/' | awk -F':' '{print $2}'| awk '{print $2}' | awk -F'[][()]' '{print $2}'| awk -F',' '{print $2<0?-$2:$2}')
    spacing_z=$(head $(data $(basename ${i})) -n 1000 | awk '/space directions:/' | awk -F':' '{print $2}'| awk '{print $3}' | awk -F'[][()]' '{print $2}'| awk -F',' '{print $3<0?-$3:$3}')

    echo $spacing_x, $spacing_y, $spacing_z
done
