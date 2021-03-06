# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
#set(PICO_SDK_PATH "/home/pi/pico/pico-sdk")

# Pull in Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(keypad_simon C CXX)

# Initialise the Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(keypad_simon keypad_simon.cpp )

pico_set_program_name(keypad_simon "keypad_simon")
pico_set_program_version(keypad_simon "0.1")

pico_enable_stdio_uart(keypad_simon 1)
pico_enable_stdio_usb(keypad_simon 0)

# Include Pimoroni libraries
include(../../pimoroni-pico/libraries/pico_rgb_keypad/pico_rgb_keypad.cmake)

# Add the standard library to the build
target_link_libraries(keypad_simon pico_stdlib pico_rgb_keypad)

pico_add_extra_outputs(keypad_simon)

