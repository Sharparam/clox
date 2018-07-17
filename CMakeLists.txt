cmake_minimum_required(VERSION 3.0.0)
project(clox
    VERSION 0.1.0
    DESCRIPTION "C implementation of the Lox scripting language"
    LANGUAGES C)

include(CTest)
enable_testing()

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(CLOX_DEBUG_TRACE_EXECUTION "Print debug information for each instruction as they're executed" OFF)

configure_file(
    "${PROJECT_SOURCE_DIR}/include/clox_config.h.in"
    "${PROJECT_BINARY_DIR}/clox_config.h")

include_directories(
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_BINARY_DIR}")

add_executable(clox
    src/main.c
    src/clox_options.c
    src/clox_chunk.c
    src/clox_memory.c
    src/clox_debug.c
    src/clox_value.c
    src/clox_vm.c)

if(MSVC)
  target_compile_options(clox PRIVATE /W4 /WX)
else(MSVC)
  target_compile_options(clox PRIVATE -Wall -Wextra -pedantic -Werror)
endif(MSVC)

install(TARGETS clox DESTINATION bin)

include(InstallRequiredSystemLibraries)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
include(CPack)
