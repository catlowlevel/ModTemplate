cmake_minimum_required(VERSION 3.10)

project(MyLibName)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -s -Wno-error=format-security -fvisibility=hidden -fpermissive -fexceptions -Wno-error=c++11-narrowing -Wall")
set(LINKER_FLAGS "-ffixed-x18 -Wl,--hash-style=both")
set(C_FLAGS "-Werror=format -fdata-sections -ffunction-sections")

# print out compiler information
message("CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
message("CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")
message("${CMAKE_BINARY_DIR}")
message("${CMAKE_CURRENT_LIST_DIR}")

set(C_FLAGS "${C_FLAGS} -O3 -fvisibility=hidden -fvisibility-inlines-hidden")
set(LINKER_FLAGS "${LINKER_FLAGS} -Wl,-exclude-libs,ALL -Wl,--gc-sections -Wl,--strip-all")

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    # set(C_FLAGS "${C_FLAGS} -O2 -fvisibility=hidden -fvisibility-inlines-hidden")
    # set(LINKER_FLAGS "${LINKER_FLAGS} -Wl,-exclude-libs,ALL -Wl,--gc-sections -Wl,--strip-all")
    set(DOBBY_DEBUG OFF)
    message("Release Mode")
else ()
    # set(C_FLAGS "${C_FLAGS} -O2")
    # set(LINKER_FLAGS "${LINKER_FLAGS}")
    message("Debug Mode")
    add_compile_definitions(__DEBUG__)
endif ()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${C_FLAGS}")

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${LINKER_FLAGS}")

set(DOBBY_GENERATE_SHARED OFF)

# Add the source files
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(LOCAL_SRC_FILES
        Main.cpp
        Menu/Menu.cpp
        Menu/Setup.cpp
        Includes/Utils.cpp
        KittyMemory/KittyMemory.cpp
        KittyMemory/MemoryPatch.cpp
        KittyMemory/MemoryBackup.cpp
        KittyMemory/KittyUtils.cpp
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

add_subdirectory(Dobby)
# Add the shared library
add_library(MyLibName SHARED ${LOCAL_SRC_FILES})

# Link required libraries
target_link_libraries(MyLibName
        log
        android
        EGL
        GLESv2
        dobby_static
        )

if(WIN32)
  # Windows platform detected, perform a copy operation
  file(COPY "${CMAKE_BINARY_DIR}/compile_commands.json" DESTINATION "${CMAKE_SOURCE_DIR}")
else()
  # For non-Windows platforms, create a symbolic link
  file(CREATE_LINK "${CMAKE_BINARY_DIR}/compile_commands.json" "${CMAKE_SOURCE_DIR}/compile_commands.json" SYMBOLIC)
endif()
