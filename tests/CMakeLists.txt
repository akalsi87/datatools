# CMakeLists.txt

# tests

file(GLOB_RECURSE src_files src *.c *.cpp)

add_executable(tests ${src_files})

target_include_directories(
  tests
  PRIVATE ${PROJECT_SOURCE_DIR}/export)

target_link_libraries(tests datatools)

add_custom_target(
  check
  COMMAND tests
  DEPENDS tests)
  
