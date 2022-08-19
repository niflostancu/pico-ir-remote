cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico_ir_remote C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if(PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

include_directories("${CMAKE_SOURCE_DIR}")
list(APPEND PICO_CONFIG_HEADER_FILES custom_config.h)

# Initialize the SDK
pico_sdk_init()

if(TARGET tinyusb_device)
  add_executable(
    pico_ir_remote
    main.c
    commands.c
    device_mgr.c
  )

  add_subdirectory(lib)

  # pull in common dependencies
  target_link_libraries(
    pico_ir_remote pico_stdlib hardware_pio
    ir_custom_tx pico_cli_lib
    )
  # workaround for USB Errata RP2040-E5
  target_compile_definitions(pico_ir_remote PUBLIC PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1)

  # enable usb output, disable uart output
  pico_enable_stdio_usb(pico_ir_remote 1)
  pico_enable_stdio_uart(pico_ir_remote 0)

  # create map/bin/hex/uf2 file etc.
  pico_add_extra_outputs(pico_ir_remote)

elseif(PICO_ON_DEVICE)
  message(WARNING "TinyUSB submodule is not initialized in the SDK")
endif()
