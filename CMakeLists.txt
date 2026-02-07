#==============================================================================
# Main CMakeLists.txt file for the Oscilloscope project
#==============================================================================

cmake_minimum_required(VERSION 3.30)
project(Disflux VERSION 1.1.2) # Disflux-Version
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS TRUE) 
set(EXT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)

# Set macOS deployment target for older macOS compatibility
if(APPLE)
    # macOS 12.0 (Monterey) is the minimum for reliable C++23 support
    set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "Minimum macOS deployment target")
endif()
include(cmake/get_cpm.cmake)

# Add external dependencies
CPMAddPackage(
    NAME JUCE
    GITHUB_REPOSITORY juce-framework/JUCE
    GIT_TAG master
    SOURCE_DIR ${EXT_DIR}/juce
)
CPMAddPackage(
    NAME CLAP_JUCE
    GITHUB_REPOSITORY free-audio/clap-juce-extensions
    GIT_TAG main
    SOURCE_DIR ${EXT_DIR}/clap
)
CPMAddPackage(
    NAME MELATONIN_PERFETTO
    GITHUB_REPOSITORY Lunix-420/melatonin_perfetto
    GIT_TAG main
    SOURCE_DIR ${EXT_DIR}/melatonin_perfetto
)
CPMAddPackage(
    NAME MELATONIN_BLUR
    GITHUB_REPOSITORY Lunix-420/melatonin_blur
    GIT_TAG main
    SOURCE_DIR ${EXT_DIR}/melatonin_blur
)

# Set C++ compiler flags
if(MSVC)
    add_compile_options(/Qspectre )#/Wall /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Silence some warnings
add_definitions(-D_SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING)

add_subdirectory(src)

