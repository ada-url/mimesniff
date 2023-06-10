add_subdirectory(wpt)

set(ADA_TEST_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../)
if(MSVC)
  add_compile_options("/Zi" "/EHsc" "/GR")
endif()

include(${PROJECT_SOURCE_DIR}/cmake/add-cpp-test.cmake)
link_libraries(ada-mimesniff)

include(GoogleTest)

add_executable(wpt_tests wpt_tests.cpp)
target_link_libraries(wpt_tests PRIVATE simdjson GTest::gtest_main)
gtest_discover_tests(wpt_tests)

add_executable(basic_tests basic_tests.cpp)
target_link_libraries(basic_tests PRIVATE simdjson GTest::gtest_main)
gtest_discover_tests(basic_tests)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
    target_link_libraries(wpt_tests PUBLIC stdc++fs)
    target_link_libraries(basic_tests PUBLIC stdc++fs)
  endif()
endif()

if(MSVC OR MINGW)
  target_compile_definitions(wpt_tests PRIVATE _CRT_SECURE_NO_WARNINGS)
  target_compile_definitions(basic_tests PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
