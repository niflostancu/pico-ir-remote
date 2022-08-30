# Makefile for easy RPI Pico FW compilation / flashing

BUILD_DIR=build
PROJECT=pico_ir_remote

CMAKE_ARGS=-DCMAKE_EXPORT_COMPILE_COMMANDS=YES

# set to 1 to force reboot before flashing
FORCE = 

build:
	@mkdir -p "$(BUILD_DIR)" && \
	ln -sf build/compile_commands.json .; \
	( cd "$(BUILD_DIR)"; cmake .. $(CMAKE_ARGS) && make; )
	

flash:
	picotool load $(if $(FORCE),-f) "$(BUILD_DIR)/$(PROJECT).elf"

clean:
	rm -rf "$(BUILD_DIR)"

.PHONY: build flash clean

