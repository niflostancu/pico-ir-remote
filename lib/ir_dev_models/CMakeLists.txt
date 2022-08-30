# IR device models library

add_library(ir_dev_models
    starlight_ac.c
)

# add the `binary` directory so that the generated headers are included in the project
target_include_directories(ir_dev_models PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ir_dev_models PUBLIC ir_proto_lib)

