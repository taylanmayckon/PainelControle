cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_BOARD pico_w CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(PainelControle C CXX ASM)

pico_sdk_init()

add_executable(PainelControle PainelControle.c )

pico_set_program_name(PainelControle "PainelControle")
pico_set_program_version(PainelControle "0.1")

pico_enable_stdio_uart(PainelControle 0)
pico_enable_stdio_usb(PainelControle 1)

target_link_libraries(PainelControle
        pico_stdlib)

target_include_directories(PainelControle PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(PainelControle 
        
        )

pico_add_extra_outputs(PainelControle)

