#==============================================================================
# Main CMakeLists.txt file for the Oscilloscope project
#==============================================================================

cmake_minimum_required(VERSION 3.30)
project(Disflux VERSION 1.1.0)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS TRUE) 
set(EXT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)
include(cmake/get_cpm.cmake)

# Add external dependencies
CPMAddPackage(
    NAME JUCE
    GITHUB_REPOSITORY juce-framework/JUCE
    GIT_TAG master
    SOURCE_DIR ${EXT_DIR}/juce
)
CPMAddPackage(
    NAME DMT
    GITHUB_REPOSITORY Dimethoxy/DMT
    GIT_TAG v25.1.2
    SOURCE_DIR ${EXT_DIR}/dmt
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
