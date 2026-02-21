cmake_minimum_required(VERSION 3.4.1)

project(vmp_tool)

set(CMAKE_CXX_STANDARD 11)

# Include directories
include_directories(engine runtime)

# Engine library (The protection tool)
add_library(vmp_engine SHARED
            engine/elf_parser.cpp
            engine/vmp_engine.cpp
            engine/string_obfuscator.cpp
            engine/jni_protector.cpp
            NativeInterface.cpp)

# Runtime Stub (To be embedded or used for security)
add_library(vmp_runtime STATIC
            runtime/linker.cpp
            runtime/vm_runtime.cpp
            runtime/decryptor.cpp
            runtime/security.cpp)

# Test library
add_library(test_lib SHARED
            test/test_lib.cpp)

# Link with log library
find_library(log-lib log)

target_link_libraries(vmp_engine ${log-lib})
target_link_libraries(test_lib ${log-lib})
