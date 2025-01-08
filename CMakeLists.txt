cmake_minimum_required(VERSION 3.19)

project(Plug64 VERSION 0.1.0)

# This is temporarily needed due to a bug in Xcode 15:
if (CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
    if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "15.0")
        add_compile_definitions(JUCE_SILENCE_XCODE_15_LINKER_WARNING=1)
        set(CMAKE_EXE_LINKER_FLAGS "-Wl,-ld_classic" CACHE INTERNAL "")
    endif ()
endif ()

# Add the CMake folder
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

# Compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Minimum MacOS target, set globally
if (${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
    set(CMAKE_OSX_DEPLOYMENT_TARGET 11.0 CACHE STRING "Minimum OS X deployment version" FORCE)
else ()
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.11" CACHE STRING "Minimum OS X deployment version" FORCE)
endif ()

option(UniversalBinary "Build universal binary for mac" OFF)

if (UniversalBinary)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE INTERNAL "")
endif ()

# Static linking in Windows
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Maximum supported channels
option(MAX_CHANS "Maximum number of channels" 64)
add_compile_definitions(MAX_CHANS=${MAX_CHANS})

# Require libraries
find_package(juce REQUIRED)

include_directories(${JUCE_INCLUDE_DIRS})

juce_add_binary_data(BinaryData SOURCES
        Resources/Font.ttf
)

add_subdirectory(Filter64)
add_subdirectory(Gain64)
