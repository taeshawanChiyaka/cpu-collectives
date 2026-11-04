cmake_minimum_required(VERSION 3.20)
project(cpu_collectives VERSION 0.1.0 LANGUAGES CXX)

option(CC_BUILD_TESTS "Build correctness and stress tests" ON)
option(CC_BUILD_BENCHMARKS "Build microbenchmarks" ON)
option(CC_BUILD_EXAMPLES "Build examples" ON)
option(CC_ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

include(cmake/cc_sanitizers.cmake)

add_library(cpu_collectives
    src/cc_status.cpp
    src/communicator.cpp
    src/sync/barrier.cpp
)
add_library(cpu_collectives::cpu_collectives ALIAS cpu_collectives)
target_compile_features(cpu_collectives PUBLIC cxx_std_20)
target_include_directories(cpu_collectives
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)
set_target_properties(cpu_collectives PROPERTIES CXX_EXTENSIONS OFF)
if(MSVC)
    target_compile_options(cpu_collectives PRIVATE /W4 /permissive-)
else()
    target_compile_options(cpu_collectives PRIVATE -Wall -Wextra -Wpedantic)
endif()
cc_enable_sanitizers(cpu_collectives)

if(CC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
if(CC_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()
if(CC_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
