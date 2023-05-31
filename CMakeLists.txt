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

if(ADA_MIMESNIFF_BENCHMARKS OR BUILD_TESTING)
  include(${PROJECT_SOURCE_DIR}/cmake/import.cmake)
  import_dependency(simdjson simdjson/simdjson 0a3a00c95665cc1fda760e121ba8d442945ede13)
  add_dependency(simdjson)
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
