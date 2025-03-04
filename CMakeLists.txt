# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
# set(PICO_SDK_PATH "/home/workstation/Instruo/pico/pico-sdk")
set(PICO_SDK_PATH "/home/sebastian/Instruo/pico/pico-sdk")

set(PICO_BOARD pico CACHE STRING "Board type")

set(PROJECT_NAME pico-template-project)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(${PROJECT_NAME} C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
set(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/inc)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)


add_executable(${PROJECT_NAME}  
    ${SOURCES}
    )

pico_set_program_name(${PROJECT_NAME}  "template")
pico_set_program_version(${PROJECT_NAME}  "0.1")

pico_enable_stdio_uart(${PROJECT_NAME}  1)
pico_enable_stdio_usb(${PROJECT_NAME}  1)

# Add the standard library to the build
target_link_libraries(${PROJECT_NAME} 
        pico_stdlib
        tinyusb_device   # Enable TinyUSB device stack
        # tinyusb_host     # Enable TinyUSB host stack if needed
        )

# Add any user requested libraries
target_link_libraries(${PROJECT_NAME}
        hardware_adc
        hardware_spi
        hardware_i2c
        hardware_pwm
        hardware_pio
        hardware_timer
        )

# Add the standard include files to the build
target_include_directories(${PROJECT_NAME}  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
  ${INCLUDE_DIRECTORIES}
  )


target_compile_definitions(${PROJECT_NAME} PUBLIC
  LOG_LEVEL=DEBUG_LEVEL
  )

  target_compile_options(${PROJECT_NAME} PRIVATE
  ${CPU_PARAMETERS}
  -Og
  )

pico_add_extra_outputs(${PROJECT_NAME})

pico_generate_pio_header(pico-template-project ${CMAKE_CURRENT_LIST_DIR}/inc/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/inc/)

