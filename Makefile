# Makefile for easy RPI Pico FW compilation / flashing

BUILD_DIR=build
PROJECT=pico_ir_remote


build:
	@mkdir -p "$(BUILD_DIR)" && \
	( cd "$(BUILD_DIR)"; cmake .. && make; )

flash:
	picotool load "$(BUILD_DIR)/$(PROJECT).elf"

clean:
	rm -rf "$(BUILD_DIR)"

.PHONY: build flash clean


