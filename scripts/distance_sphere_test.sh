#!/usr/bin/env bash

# Loading the environment variables
source environment.sh

# Testing sphere generation
${SPHERE_GENERATOR} -z 100 -s 1 -r 20 -o ${TEST_TMP_DIR}/sphere_100_1_20.nii
