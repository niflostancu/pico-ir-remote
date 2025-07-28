# IR protocol decoder library

add_library(ir_decoder demod_pdm.c)

target_link_libraries(ir_decoder PUBLIC pico_stdlib pico_float ir_capture)
