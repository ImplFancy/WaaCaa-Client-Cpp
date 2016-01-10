cmake_minimum_required(VERSION 2.4)

include_directories(.)

aux_source_directory(model src_model)
aux_source_directory(network src_network)
aux_source_directory(utils src_utils)

add_definitions(-DUNICODE -D_UNICODE)
#add_definitions(-DWAACAACLIENT_EXPORTS)

add_library(${library_name} STATIC ${src_model} ${src_network} ${src_utils})
