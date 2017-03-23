# CMakeLists.txt for install

cmake_minimum_required(VERSION 3.0)

set(CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX})

file(GLOB src_files ${TESTS_DIR}/*.c ${TESTS_DIR}/*.cpp)

find_package(datatools)

add_executable(tests_install ${src_files})

target_include_directories(
  tests_install
  PRIVATE ${PROJECT_SOURCE_DIR}/export)

target_link_libraries(tests_install datatools)

add_custom_target(
  tests_run
  COMMAND tests_install
  DEPENDS tests_install)
