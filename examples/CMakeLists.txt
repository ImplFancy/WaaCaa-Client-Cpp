cmake_minimum_required(VERSION 2.4)

include_directories(../library/model/)

list(APPEND example_list example_01)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    list(APPEND example_list example_02)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")


foreach(example_name ${example_list})
    add_executable(${example_name} ${example_name}.cpp)
    target_link_libraries(${example_name} ${library_name})
endforeach(example_name ${example_list})