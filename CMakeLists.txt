cmake_minimum_required(VERSION 3.16)

project(ada-mimesniff
  DESCRIPTION "MIMESNIFF"
  LANGUAGES C CXX
  VERSION 0.1.0
)

include(GNUInstallDirs)

include(CTest)
include(cmake/mimesniff-flags.cmake)

add_subdirectory(src)

include(cmake/clang-format.cmake)

option(ADA_MIMESNIFF_BENCHMARKS "Build benchmarks" OFF)

# We use Google Benchmark, but it does not build under several 32-bit systems.
if((BUILD_TESTING OR ADA_MIMESNIFF_BENCHMARKS) AND (CMAKE_SIZEOF_VOID_P EQUAL 8))
  include(${PROJECT_SOURCE_DIR}/cmake/import.cmake)
  import_dependency(simdjson simdjson/simdjson 14d927128ba14e13913fc0e7c2cf538790bd1622)
  add_dependency(simdjson)
  import_dependency(gtest google/googletest 797b0ad2a3a45608ecf5c67e6e289d377a3521ca)
  add_dependency(gtest)
endif()

if (ADA_MIMESNIFF_BENCHMARKS)
  message(STATUS "Ada benchmarks enabled.")
  add_subdirectory(benchmarks)
else(ADA_MIMESNIFF_BENCHMARKS)
  if (is_top_project)
    message(STATUS "Ada mimesniff benchmarks disabled. Set ADA_MIMESNIFF_BENCHMARKS=ON to enable them.")
  endif()
endif()

if (BUILD_TESTING)
  message(STATUS "The tests are enabled.")
  add_subdirectory(tests)
else()
  if(is_top_project)
    message(STATUS "The tests are disabled.")
  endif()
endif(BUILD_TESTING)

add_subdirectory(singleheader)
