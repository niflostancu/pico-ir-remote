# IR Transmit Library

add_library(ir_custom_tx ir_cust_tx.c)

# invoke pio_asm to assemble the PIO state machine programs
pico_generate_pio_header(ir_custom_tx ${CMAKE_CURRENT_LIST_DIR}/ir_cust_nec_burst.pio)
pico_generate_pio_header(ir_custom_tx ${CMAKE_CURRENT_LIST_DIR}/ir_cust_nec_control.pio)

target_link_libraries(ir_custom_tx PRIVATE
        pico_stdlib
        hardware_pio
        )

# add the `binary` directory so that the generated headers are included in the project
target_include_directories (ir_custom_tx PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	)

