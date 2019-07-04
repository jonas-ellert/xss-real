#
add_executable(benchmark ${CMAKE_CURRENT_SOURCE_DIR}/bench.cpp)
target_link_libraries(benchmark tlx_command_line stdc++fs gsaca divsufsort divsufsort64 sdsl malloc_count rt dl)

add_executable(generator ${CMAKE_CURRENT_SOURCE_DIR}/gen.cpp)
target_link_libraries(generator tlx_command_line stdc++fs rt dl)