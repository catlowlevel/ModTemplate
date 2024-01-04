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
    set(USE_FRIDA OFF)  # Disable Frida in Release mode
    message("Release Mode")
else ()
    # set(C_FLAGS "${C_FLAGS} -O2")
    # set(LINKER_FLAGS "${LINKER_FLAGS}")
    set(USE_FRIDA ON)   # Enable Frida in Debug mode
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

set(JSON_BuildTests OFF CACHE INTERNAL "")

add_subdirectory(json)

if(USE_FRIDA)
    include_directories(${CMAKE_SOURCE_DIR}/Frida/${CMAKE_ANDROID_ARCH_ABI})
    include_directories(${CMAKE_SOURCE_DIR}/Frida/gumpp)
    set(LOCAL_SRC_FILES ${LOCAL_SRC_FILES}
        Frida/gumpp/runtime.cpp
        Frida/gumpp/backtracer.cpp
        Frida/gumpp/interceptor.cpp
        Frida/gumpp/invocationlistener.cpp
        Frida/gumpp/returnaddress.cpp
    )
    add_library(MyLibName SHARED ${LOCAL_SRC_FILES})
    target_link_libraries(MyLibName
        log
        android
        EGL
        GLESv2
        dobby_static
        nlohmann_json::nlohmann_json
        ${CMAKE_SOURCE_DIR}/Frida/${CMAKE_ANDROID_ARCH_ABI}/libfrida-gum.a
    )
else()
    add_library(MyLibName SHARED ${LOCAL_SRC_FILES})
    target_link_libraries(MyLibName
        log
        android
        EGL
        GLESv2
        dobby_static
        nlohmann_json::nlohmann_json
    )
endif()

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
  # Windows platform detected, perform a copy operation
  file(COPY "${CMAKE_BINARY_DIR}/compile_commands.json" DESTINATION "${CMAKE_SOURCE_DIR}")
else()
  # For non-Windows platforms, create a symbolic link
  file(CREATE_LINK "${CMAKE_BINARY_DIR}/compile_commands.json" "${CMAKE_SOURCE_DIR}/compile_commands.json" SYMBOLIC)
endif()
