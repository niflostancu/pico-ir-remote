# Add all supported protocols

add_library(ir_proto_lib ir_utils.c)

# add this directory to include path
target_include_directories(ir_proto_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(nec_custom)

target_link_libraries(ir_proto_lib PUBLIC ir_proto_nec_custom)

