#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "aws-cpp-sdk-s3" for configuration "Debug"
set_property(TARGET aws-cpp-sdk-s3 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(aws-cpp-sdk-s3 PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/debug/lib/aws-cpp-sdk-s3.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBUG "aws-cpp-sdk-core"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/bin/aws-cpp-sdk-s3.dll"
  )

list(APPEND _cmake_import_check_targets aws-cpp-sdk-s3 )
list(APPEND _cmake_import_check_files_for_aws-cpp-sdk-s3 "${_IMPORT_PREFIX}/debug/lib/aws-cpp-sdk-s3.lib" "${_IMPORT_PREFIX}/debug/bin/aws-cpp-sdk-s3.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
