#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "aws-cpp-sdk-sns" for configuration "Debug"
set_property(TARGET aws-cpp-sdk-sns APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(aws-cpp-sdk-sns PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/debug/lib/aws-cpp-sdk-sns.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBUG "aws-cpp-sdk-core"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/bin/aws-cpp-sdk-sns.dll"
  )

list(APPEND _cmake_import_check_targets aws-cpp-sdk-sns )
list(APPEND _cmake_import_check_files_for_aws-cpp-sdk-sns "${_IMPORT_PREFIX}/debug/lib/aws-cpp-sdk-sns.lib" "${_IMPORT_PREFIX}/debug/bin/aws-cpp-sdk-sns.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)