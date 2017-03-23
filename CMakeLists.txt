# CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

project(datatools)

# LIBRARY: libdata-tools

file(GLOB_RECURSE export_hdr export *.h *.hpp)

file(GLOB_RECURSE src_files src *.c *.cpp)

set(ver_maj 0)
set(ver_min 1)
set(ver_patch 0)
set(ver_string ${ver_maj}.${ver_min}.${ver_patch})

add_library(datatools ${src_files})

if (BUILD_SHARED_LIBS)
  set(build_flag BUILDING_DATATOOLS)
else()
  set(build_flag )
endif()

target_compile_definitions(
  datatools
  PRIVATE
    ${build_flag}
    DATATOOLS_VER_MAJ=${ver_maj}
    DATATOOLS_VER_MIN=${ver_min}
    DATATOOLS_VER_PATCH=${ver_patch}
    DATATOOLS_VER_STRING=${ver_string})

target_include_directories(
  datatools
  PRIVATE include
  PUBLIC export)

target_link_libraries(datatools )

target_compile_features(
  datatools
  PUBLIC
  cxx_nullptr
  cxx_lambdas
  cxx_auto_type
  cxx_static_assert)

# INSTALL

install(
  TARGETS datatools
  EXPORT datatoolsConfig.cmake
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include)

install(
  DIRECTORY export/
  DESTINATION include)

# install configs

set(cmake_config_dir lib/cmake/datatools)

# TESTS

add_subdirectory(tests)
