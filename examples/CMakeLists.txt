cmake_minimum_required(VERSION 2.4)

include_directories(../library/model/)

foreach(example_name example_01) # example_02)
    add_executable(${example_name} ${example_name}.cpp)
    target_link_libraries(${example_name} ${library_name})
endforeach(example_name)