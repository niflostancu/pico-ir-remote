# Main firmware components

add_library(main_lib
    commands.c
    device_mgr.c
    scheduler.c
)

target_link_libraries(main_lib
    PRIVATE pico_stdlib ir_dev_models pico_cli_lib)

