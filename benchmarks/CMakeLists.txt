# Bench
add_executable(wpt_bench wpt_bench.cpp)
target_link_libraries(wpt_bench PRIVATE ada-mimesniff)
target_link_libraries(wpt_bench PRIVATE simdjson)
target_include_directories(wpt_bench PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>")
target_include_directories(wpt_bench PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/benchmarks>")

include(${PROJECT_SOURCE_DIR}/cmake/import.cmake)

set_off(BENCHMARK_ENABLE_TESTING)
set_off(BENCHMARK_ENABLE_INSTALL)
set_off(BENCHMARK_ENABLE_WERROR)

import_dependency(google_benchmarks google/benchmark f91b6b4)
add_dependency(google_benchmarks)
target_link_libraries(wpt_bench PRIVATE benchmark::benchmark)

