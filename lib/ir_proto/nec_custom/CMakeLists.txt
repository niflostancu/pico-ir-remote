# IR protocol library for a NEC-like protocol with custom timing

add_library(ir_proto_nec_custom nec_custom_proto.c)

# invoke pio_asm to assemble the PIO state machine programs
pico_generate_pio_header(ir_proto_nec_custom ${CMAKE_CURRENT_LIST_DIR}/cust_nec_burst.pio)
pico_generate_pio_header(ir_proto_nec_custom ${CMAKE_CURRENT_LIST_DIR}/cust_nec_control.pio)

target_link_libraries(ir_proto_nec_custom PUBLIC
    pico_stdlib
    hardware_pio
)

# add the `binary` directory so that the generated headers are included in the project
target_include_directories (ir_proto_nec_custom PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

