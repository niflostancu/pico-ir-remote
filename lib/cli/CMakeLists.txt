# Stdio-based CLI library for embedded programs

add_library(pico_cli_lib cli.c)

target_link_libraries(pico_cli_lib PRIVATE pico_stdlib)

target_include_directories(pico_cli_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

