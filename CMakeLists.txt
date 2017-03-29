# CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

project(datatools)

set(ver_maj 0)
set(ver_min 1)
set(ver_patch 0)
set(ver_string ${ver_maj}.${ver_min}.${ver_patch})

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

if (NOT WIN32)
  set(archive_dest lib)
  set(library_dest lib)
  set(runtime_dest bin)
  set(include_dest include)
  set(cmake_dest lib/cmake)
  set(license_dest share/datatools)
else()
  set(archive_dest lib)
  set(library_dest lib)
  set(runtime_dest .)
  set(include_dest include)
  set(cmake_dest lib/cmake/datatools)
  set(license_dest .)
endif()

set(PROJECT_VERSION ${ver_string})

# LIBRARY: libdata-tools

macro(msg m)
  message("## [datatools] ${m}")
endmacro(msg)

msg("Got project source dir: ${PROJECT_SOURCE_DIR}")

file(GLOB_RECURSE export_hdr ${PROJECT_SOURCE_DIR}/export/*.h ${PROJECT_SOURCE_DIR}/*.hpp)

file(GLOB_RECURSE src_files ${PROJECT_SOURCE_DIR}/src/*.c ${PROJECT_SOURCE_DIR}/src/*.cpp)

msg("Got export hdrs: ${export_hdr}")
msg("Got src files: ${src_files}")

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
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/export>
         $<INSTALL_INTERFACE:${include_dest}>)

target_link_libraries(datatools )

target_compile_features(
  datatools
  PUBLIC
  cxx_nullptr
  cxx_lambdas
  cxx_auto_type
  cxx_static_assert)

configure_file(
  cmake/datatoolsConfig.cmake.in
  ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/datatoolsConfig.cmake @ONLY)

# INSTALL

export(
  TARGETS datatools
  FILE ${PROJECT_BINARY_DIR}/datatoolsTargets.cmake)

export(PACKAGE datatools)

# install configs

install(
  TARGETS datatools
  EXPORT datatoolsTargets
  ARCHIVE DESTINATION ${archive_dest}
  LIBRARY DESTINATION ${library_dest}
  RUNTIME DESTINATION ${runtime_dest}
  INCLUDES DESTINATION ${include_dest})

install(
  DIRECTORY export/
  DESTINATION ${include_dest})

set(cmake_config_dir ${cmake_dest})

install(
  FILES ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/datatoolsConfig.cmake
  DESTINATION ${cmake_config_dir}
  COMPONENT dev)

install(
  EXPORT datatoolsTargets
  DESTINATION ${cmake_config_dir}
  COMPONENT dev)

install(
  FILES ${PROJECT_SOURCE_DIR}/LICENSE
  DESTINATION ${license_dest})

# TESTS

include(tests.cmake)
