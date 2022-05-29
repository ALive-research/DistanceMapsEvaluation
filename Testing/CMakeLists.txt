#------------------------------------------------------------------------------
# File names
#------------------------------------------------------------------------------
set(SPHERE_FILE_NAME "sphere_101_1_20")
set(SPHERE_MAURER_FILE_NAME "sphere_101_1_20_maurer")
set(SPHERE_DISTANCE_FILE_NAME "sphere_101_1_20_base")
set(SPHERE_CROPPED_FILE_NAME "sphere_101_1_20_cropped")
set(SPHERE_PADDED_FILE_NAME "sphere_141_1_20")
set(SPHERE_RADIAL_FILE_NAME "sphere_101_1_20_radial")
set(SPHERE_NRRD_FLOAT_FILE_NAME "sphere_101_1_20_float")
set(HEPATIC_VESSELS_FILE_NAME "hepaticvessel_151")

#------------------------------------------------------------------------------
# Generate a sphere
#------------------------------------------------------------------------------
ExternalData_Add_Test(Test_itkSphereImageGenerator
  NAME Test_itkSphereImageGenerator
  COMMAND  ${SPHERE_GENERATOR} -z 101 -s 1 -r 20 -o ${TEST_TMP_DIR}/${SPHERE_FILE_NAME}.nii
  )

ExternalData_Add_Target(Test_itkSphereImageGenerator)

#------------------------------------------------------------------------------
# Compare sphere with reference
#------------------------------------------------------------------------------
ExternalData_Add_Test(Test_itkSphereCompare
  NAME Test_itkSphereCompare
  COMMAND  ${COMPARE_OPERATOR} -a ${TEST_TMP_DIR}/${SPHERE_FILE_NAME}.nii -b DATA{${SPHERE_FILE_NAME}.nii}
  )

ExternalData_Add_Target(Test_itkSphereCompare)

set_tests_properties(Test_itkSphereCompare PROPERTIES DEPENDS "Test_itkSphereImageGenerator")

#------------------------------------------------------------------------------
# Test Maurer distance
#------------------------------------------------------------------------------
ExternalData_Add_Test(Test_itkMaurerDistance
  NAME Test_itkMaurerDistance
  COMMAND  ${MAURER_OPERATOR} -i ${TEST_TMP_DIR}/${SPHERE_FILE_NAME}.nii -o ${TEST_TMP_DIR}/${SPHERE_MAURER_FILE_NAME}.nii
  )

ExternalData_Add_Target(Test_itkMaurerDistance)

set_tests_properties(Test_itkMaurerDistance PROPERTIES DEPENDS "Test_itkSphereImageGenerator")

#------------------------------------------------------------------------------
# Test Base distance
#------------------------------------------------------------------------------
ExternalData_Add_Test(Test_itkDistanceImageFilter
  NAME Test_itkDistanceImageFilter
  COMMAND  ${DISTANCE_OPERATOR} -i ${TEST_TMP_DIR}/${SPHERE_FILE_NAME}.nii -o ${TEST_TMP_DIR}/${SPHERE_DISTANCE_FILE_NAME}.nii
  )

ExternalData_Add_Target(Test_itkDistanceImageFilter)

set_tests_properties(Test_itkDistanceImageFilter PROPERTIES DEPENDS "Test_itkSphereImageGenerator")

#------------------------------------------------------------------------------
# Compare Maurer and base
#------------------------------------------------------------------------------
ExternalData_Add_Test(Test_itkSphereMaurerBaseCompare
  NAME Test_itkSphereMaurerBaseCompare
  COMMAND  ${COMPARE_OPERATOR} -a ${TEST_TMP_DIR}/${SPHERE_MAURER_FILE_NAME}.nii -b ${TEST_TMP_DIR}/${SPHERE_DISTANCE_FILE_NAME}.nii
  )

ExternalData_Add_Target(Test_itkSphereMaurerBaseCompare)

set_tests_properties(Test_itkSphereMaurerBaseCompare PROPERTIES DEPENDS "Test_itkSphereImageGenerator")

#------------------------------------------------------------------------------
# Generate radial distance image
#------------------------------------------------------------------------------
ExternalData_Add_Test(Test_itkRadialDistance
  NAME Test_itkRadialDistance
  COMMAND  ${RADIAL_OPERATOR} -z 101 -s 1 -r 20 -o ${TEST_TMP_DIR}/${SPHERE_RADIAL_FILE_NAME}.nii
  )

ExternalData_Add_Target(Test_itkRadialDistance)

#------------------------------------------------------------------------------
# Compare radial with base
#------------------------------------------------------------------------------
ExternalData_Add_Test(Test_itkRadialBaseDistanceCompare
  NAME Test_itkRadialBaseDistanceCompare
  COMMAND  ${COMPARE_OPERATOR} -a ${TEST_TMP_DIR}/${SPHERE_RADIAL_FILE_NAME}.nii -b ${TEST_TMP_DIR}/${SPHERE_DISTANCE_FILE_NAME}.nii -k ${TEST_TMP_DIR}/${SPHERE_FILE_NAME}.nii -o -m 1.0 -M 1.0 -s 1.0 -e 1.0
  )

ExternalData_Add_Target(Test_itkRadialBaseDistanceCompare)
set_tests_properties(Test_itkRadialBaseDistanceCompare PROPERTIES DEPENDS "Test_itkRadialDistance;Test_itkDistanceImageFilter")

#------------------------------------------------------------------------------
# Compare Maurer with base
#------------------------------------------------------------------------------
ExternalData_Add_Test(Test_itkRadialMaurerDistanceCompare
  NAME Test_itkRadialMaurerDistanceCompare
  COMMAND  ${COMPARE_OPERATOR} -a ${TEST_TMP_DIR}/${SPHERE_RADIAL_FILE_NAME}.nii -b ${TEST_TMP_DIR}/${SPHERE_DISTANCE_FILE_NAME}.nii -k ${TEST_TMP_DIR}/${SPHERE_FILE_NAME}.nii -o -m 1.0 -M 1.0 -s 1.0 -e 1.0
  )

ExternalData_Add_Target(Test_itkRadialMaurerDistanceCompare)
set_tests_properties(Test_itkRadialBaseDistanceCompare PROPERTIES DEPENDS "Test_itkRadialDistance;Test_itkMaurerDistance")

#------------------------------------------------------------------------------
# Crop sphere labelmap to the bounding box
#------------------------------------------------------------------------------
ExternalData_Add_Test(Test_itkBoundingBox
  NAME Test_itkBoundingBox
  COMMAND  ${BOUNDINGBOX_OPERATOR} -o ${TEST_TMP_DIR}/${SPHERE_CROPPED_FILE_NAME}.nii -i DATA{${SPHERE_FILE_NAME}.nii}
  )

ExternalData_Add_Target(Test_itkBoundingBox)

set_tests_properties(Test_itkBoundingBox PROPERTIES DEPENDS "Test_itkSphereImageGenerator")

#------------------------------------------------------------------------------
# Check crop comparison
#------------------------------------------------------------------------------
ExternalData_Add_Test(Test_itkBoundingBoxCompare
  NAME Test_itkBoundingBoxCompare
  COMMAND  ${COMPARE_OPERATOR} -a ${TEST_TMP_DIR}/${SPHERE_CROPPED_FILE_NAME}.nii -b DATA{${SPHERE_CROPPED_FILE_NAME}.nii}
  )

ExternalData_Add_Target(Test_itkBoundingBoxCompare)
set_tests_properties(Test_itkBoundingBoxCompare PROPERTIES DEPENDS "Test_itkBoundingBox")

#------------------------------------------------------------------------------
# Convert unsigned short (.nii) to float (.nrrd)
#------------------------------------------------------------------------------
ExternalData_Add_Test(Test_itkImageConvert
  NAME Test_itkImageConvert
  COMMAND  ${CONVERT_OPERATOR} -o ${TEST_TMP_DIR}/${SPHERE_NRRD_FLOAT_FILE_NAME}.nrrd -i DATA{${SPHERE_FILE_NAME}.nii} -d 2
  )

ExternalData_Add_Target(Test_itkImageConvert)

set_tests_properties(Test_itkImageConvert PROPERTIES DEPENDS "Test_itkSphereImageGenerator")

#------------------------------------------------------------------------------
# Check conversion comparison
#------------------------------------------------------------------------------
ExternalData_Add_Test(Test_itkConversionCompare
  NAME Test_itkConversionCompare
  COMMAND  ${COMPARE_OPERATOR} -a ${TEST_TMP_DIR}/${SPHERE_NRRD_FLOAT_FILE_NAME}.nrrd -b DATA{${SPHERE_NRRD_FLOAT_FILE_NAME}.nrrd}
  )

ExternalData_Add_Target(Test_itkConversionCompare)
set_tests_properties(Test_itkConversionCompare PROPERTIES DEPENDS "Test_itkImageConvert")

#------------------------------------------------------------------------------
# Extract labels
#------------------------------------------------------------------------------
ExternalData_Add_Test(Test_itkLabelExtract
  NAME Test_itkLabelExtract
  COMMAND  ${LABELEXTRACT_OPERATOR} -i DATA{${HEPATIC_VESSELS_FILE_NAME}.nii.gz} -l 1 -l 2 -L 3 -o ${TEST_TMP_DIR}/${HEPATIC_VESSELS_FILE_NAME}_unified.nii
  )

ExternalData_Add_Target(Test_itkLabelExtract)

#------------------------------------------------------------------------------
# Check conversion comparison
#------------------------------------------------------------------------------
ExternalData_Add_Test(Test_itkLabelExtractionCheck
  NAME Test_itkLabelExtractionCheck
  COMMAND  ${COMPARE_OPERATOR} -a ${TEST_TMP_DIR}/${HEPATIC_VESSELS_FILE_NAME}_unified.nii -b DATA{${HEPATIC_VESSELS_FILE_NAME}_unified.nii}
  )

ExternalData_Add_Target(Test_itkLabelExtractionCheck)
set_tests_properties(Test_itkConversionCompare PROPERTIES DEPENDS "Test_itkLabelExtract")

#------------------------------------------------------------------------------
# Crop sphere image
#------------------------------------------------------------------------------
ExternalData_Add_Test(Test_itkImageCrop
  NAME Test_itkImageCrop
  COMMAND  ${CROP_OPERATOR} -i ${TEST_TMP_DIR}/${SPHERE_FILE_NAME}.nii -x 31 -y 31 -z 31 -u 40 -v 40 -w 40 -o ${TEST_TMP_DIR}/${SPHERE_CROPPED_FILE_NAME}_2.nii
  )

ExternalData_Add_Target(Test_itkImageCrop)
set_tests_properties(Test_itkImageCrop PROPERTIES DEPENDS "Test_itkSphereImageGenerator")

#------------------------------------------------------------------------------
# Check conversion comparison
#------------------------------------------------------------------------------
ExternalData_Add_Test(Test_itkImageCropCheck
  NAME Test_itkImageCropCheck
  COMMAND  ${COMPARE_OPERATOR} -a ${TEST_TMP_DIR}/${SPHERE_CROPPED_FILE_NAME}_2.nii -b ${TEST_TMP_DIR}/${SPHERE_CROPPED_FILE_NAME}.nii
  )

ExternalData_Add_Target(Test_itkImageCropCheck)
set_tests_properties(Test_itkImageCropCheck PROPERTIES DEPENDS "Test_itkImageCrop;Test_itkBoundingBox")

#------------------------------------------------------------------------------
# Pad sphere image
#------------------------------------------------------------------------------
ExternalData_Add_Test(Test_itkImagePad
  NAME Test_itkImagePad
  COMMAND  ${PAD_OPERATOR} -i ${TEST_TMP_DIR}/${SPHERE_FILE_NAME}.nii -p 20 -v 0 -o ${TEST_TMP_DIR}/${SPHERE_PADDED_FILE_NAME}.nii
  )

ExternalData_Add_Target(Test_itkImagePad)
set_tests_properties(Test_itkImagePad PROPERTIES DEPENDS "Test_itkSphereImageGenerator")

#------------------------------------------------------------------------------
# Check conversion comparison
#------------------------------------------------------------------------------
ExternalData_Add_Test(Test_itkImagePadCheck
  NAME Test_itkImagePadCheck
  COMMAND  ${COMPARE_OPERATOR} -a ${TEST_TMP_DIR}/${SPHERE_PADDED_FILE_NAME}.nii -b DATA{${SPHERE_PADDED_FILE_NAME}.nii}
  )

ExternalData_Add_Target(Test_itkImagePadCheck)
set_tests_properties(Test_itkImagePadCheck PROPERTIES DEPENDS "Test_itkImagePad")
