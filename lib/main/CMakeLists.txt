# Main firmware components

add_library(main_lib
    device_mgr.c
    scheduler.c
)

add_subdirectory(./commands)

target_link_libraries(main_lib PUBLIC main_commands)
target_link_libraries(main_lib
    PRIVATE pico_stdlib ir_capture ir_dev_models)

