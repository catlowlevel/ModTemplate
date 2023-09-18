cmake_minimum_required(VERSION 3.10)

project(MyLibName)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -s -Wno-error=format-security -fvisibility=hidden -fpermissive -fexceptions -Wno-error=c++11-narrowing -Wall")

# Add the source files
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(LOCAL_SRC_FILES
        Main.cpp
        Substrate/hde64.c
        Substrate/SubstrateDebug.cpp
        Substrate/SubstrateHook.cpp
        Substrate/SubstratePosixMemory.cpp
        Substrate/SymbolFinder.cpp
        KittyMemory/KittyMemory.cpp
        KittyMemory/MemoryPatch.cpp
        KittyMemory/MemoryBackup.cpp
        KittyMemory/KittyUtils.cpp
        And64InlineHook/And64InlineHook.cpp
        Il2cpp/Il2cpp.cpp
        Il2cpp/il2cpp-class.cpp
        Il2cpp/xdl/xdl.c
        Il2cpp/xdl/xdl_iterate.c
        Il2cpp/xdl/xdl_linker.c
        Il2cpp/xdl/xdl_lzma.c
        Il2cpp/xdl/xdl_util.c
        )

# Add include directories
include_directories(${SRC_DIR})

# Add the shared library
add_library(MyLibName SHARED ${LOCAL_SRC_FILES})

# Link required libraries
target_link_libraries(MyLibName
        log
        android
        EGL
        GLESv2
        )