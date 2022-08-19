# Stdio-based CLI library for embedded programs

add_library(pico_cli_lib cli.c)

target_link_libraries(pico_cli_lib PRIVATE
	pico_stdlib
	)

# add the `binary` directory so that the generated headers are included in the project
target_include_directories(pico_cli_lib PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	)

