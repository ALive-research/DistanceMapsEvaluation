#------------------------------------------------------------------------------
# File names
#------------------------------------------------------------------------------
set(SPHERE_FILE_NAME "sphere_101_1_20")
set(SPHERE_MAURER_FILE_NAME "sphere_101_1_20_maurer")
set(SPHERE_BASE_FILE_NAME "sphere_101_1_20_base")
set(SPHERE_RADIAL_FILE_NAME "sphere_101_1_20_radial")

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
  COMMAND  ${IMAGE_COMPARE} -a ${TEST_TMP_DIR}/${SPHERE_FILE_NAME}.nii -b DATA{${SPHERE_FILE_NAME}.nii}
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
  COMMAND  ${BASE_OPERATOR} -i ${TEST_TMP_DIR}/${SPHERE_FILE_NAME}.nii -o ${TEST_TMP_DIR}/${SPHERE_BASE_FILE_NAME}.nii
  )

ExternalData_Add_Target(Test_itkDistanceImageFilter)

set_tests_properties(Test_itkDistanceImageFilter PROPERTIES DEPENDS "Test_itkSphereImageGenerator")

#------------------------------------------------------------------------------
# Compare Maurer and base
#------------------------------------------------------------------------------
ExternalData_Add_Test(Test_itkSphereMaurerBaseCompare
  NAME Test_itkSphereMaurerBaseCompare
  COMMAND  ${IMAGE_COMPARE} -a ${TEST_TMP_DIR}/${SPHERE_MAURER_FILE_NAME}.nii -b ${TEST_TMP_DIR}/${SPHERE_BASE_FILE_NAME}.nii
  )

ExternalData_Add_Target(Test_itkSphereMaurerBaseCompare)

set_tests_properties(Test_itkSphereMaurerBaseCompare PROPERTIES DEPENDS "Test_itkSphereImageGenerator")

#------------------------------------------------------------------------------
# Generate radial distance image
#------------------------------------------------------------------------------
ExternalData_Add_Test(Test_itkRadialDistance
  NAME Test_itkRadialDistance
  COMMAND  ${RADIAL_DISTANCE} -z 101 -s 1 -r 20 -o ${TEST_TMP_DIR}/${SPHERE_RADIAL_FILE_NAME}.nii
  )

ExternalData_Add_Target(Test_itkRadialDistance)
