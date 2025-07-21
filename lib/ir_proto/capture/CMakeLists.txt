# IR protocol library for a NEC-like protocol with custom timing

add_library(ir_capture ircapture.c)

target_link_libraries(ir_capture PUBLIC
    pico_stdlib hardware_timer ir_proto_lib
)

target_include_directories (ir_capture PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

