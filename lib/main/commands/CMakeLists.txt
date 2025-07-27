# CLI menus

add_library(main_commands
    main_cli.c
    ircap_cli.c
)

target_link_libraries(main_commands
    PUBLIC pico_stdlib ir_capture ir_dev_models rp_cli rp_cli_rp_stdio)

