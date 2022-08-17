# RaspberryPI Pico-based IR remote tool for Air Conditioners

This project aims to be a programmable InfraRed-based remote control for various
Air Conditioning systems with CLI, time-based on/off scheduling and
Internet-issued command (for the Pico W) features.

## Pre-requisites

The [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk) and all of
its dependencies needs to be properly set up.

### Hardware

You need an IR LED properly connected to pin 1 (though configurable inside
`config.h`).
Don't forget the current limiting resistor!

## Building

A `Makefile` is included with this project with the most common commands for
development and testing:

```bash
# Compile the firmware binary:
make build
# Flash it using picotool:
make flash
```

By default, it creates a `build/` directory inside the project's directory and
invokes `cmake` there (i.e., the usual Pico SDK workflow).

