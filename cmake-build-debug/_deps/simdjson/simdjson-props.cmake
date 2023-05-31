target_include_directories("${target}" ${public} [==[$<BUILD_INTERFACE:/dependencies/.cache/simdjson/include>]==] ${private} [==[$<BUILD_INTERFACE:/dependencies/.cache/simdjson/src>]==])
target_compile_features("${target}" ${public} [==[cxx_std_11]==])
target_link_libraries("${target}" ${public} [==[Threads::Threads]==])
target_compile_definitions("${target}" ${public} [==[SIMDJSON_THREADS_ENABLED=1]==])
