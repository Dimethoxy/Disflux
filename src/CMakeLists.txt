#==============================================================================
# Oscilloscope Source Folder CMakeLists.txt file
#==============================================================================

cmake_minimum_required(VERSION 3.22)
project(DisfluxPlugin VERSION 0.1.0)

# If we are on MacOS, we need to build for arm64 and x86_64
if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()

# JUCE setup
juce_add_plugin(${PROJECT_NAME}
    PRODUCT_NAME "Disflux"
    COMPANY_NAME "Dimethoxy"
    FORMATS VST3 LV2 AU Standalone  
    VST3_CATEGORIES "Fx" "Analyzer"                   
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    PLUGIN_MANUFACTURER_CODE DMTX
    PLUGIN_CODE DFLX
    LV2URI "https://dimethoxy.com/plugins/disflux"      
    BUNDLE_ID com.dimethoxy.disflux           
    )

# Add the main source files
target_sources(${PROJECT_NAME}
    PRIVATE
        app/PluginEditor.cpp
        app/PluginProcessor.cpp
)

# Add the external libraries
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/external/dmt
        ${CMAKE_SOURCE_DIR}/external/melatonin_perfetto
        ${CMAKE_SOURCE_DIR}/external/tomlplusplus
)

# Add fonts and icons as binary data so they can be used in the app
juce_add_binary_data(FontBinaryData
    SOURCES
    ${CMAKE_SOURCE_DIR}/fonts/poppins/Poppins-Thin.ttf
    ${CMAKE_SOURCE_DIR}/fonts/poppins/Poppins-ExtraLight.ttf
    ${CMAKE_SOURCE_DIR}/fonts/poppins/Poppins-Light.ttf
    ${CMAKE_SOURCE_DIR}/fonts/poppins/Poppins-Regular.ttf
    ${CMAKE_SOURCE_DIR}/fonts/poppins/Poppins-Medium.ttf
    ${CMAKE_SOURCE_DIR}/fonts/poppins/Poppins-SemiBold.ttf
    ${CMAKE_SOURCE_DIR}/fonts/poppins/Poppins-Bold.ttf
    ${CMAKE_SOURCE_DIR}/fonts/poppins/Poppins-ExtraBold.ttf
    ${CMAKE_SOURCE_DIR}/fonts/poppins/Poppins-Black.ttf
    ${CMAKE_SOURCE_DIR}/fonts/sedgwick_ave_display/SedgwickAveDisplay-Regular.ttf
    ${CMAKE_SOURCE_DIR}/icons/speed.svg
    ${CMAKE_SOURCE_DIR}/icons/back.svg
    ${CMAKE_SOURCE_DIR}/icons/height.svg
    ${CMAKE_SOURCE_DIR}/icons/thickness.svg
    ${CMAKE_SOURCE_DIR}/icons/gear.svg
    ${CMAKE_SOURCE_DIR}/icons/angles_up.svg
    ${CMAKE_SOURCE_DIR}/icons/bypass.svg
)

# Link the libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_cryptography
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_opengl
        juce::juce_product_unlocking
        Melatonin::Perfetto
        melatonin_blur
    PUBLIC
        FontBinaryData
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Conditionally link curl only on Linux
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            curl
    )
endif()

# Set up JUCE flags
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        JUCE_USE_CURL=$<BOOL:${UNIX}>
        JUCE_WEB_BROWSER=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        REPORT_JUCE_APP_USAGE=0)

# Generate JUCE header file
juce_generate_juce_header(${PROJECT_NAME})
message("JUCE header file generated!")

# Set up compiler flags for MSVC
if(MSVC)
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
            _SILENCE_CXX23_ALIGEND_STORAGE_DEPRECATION_WARNING
    )
endif()

# Set up OS flags to use in code later
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        OS_IS_WINDOWS=$<BOOL:${WIN32}>
        OS_IS_DARWIN=$<BOOL:${APPLE}>
        OS_IS_LINUX=$<BOOL:${UNIX} AND NOT APPLE>
)

# CLAP support setup
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/..)
clap_juce_extensions_plugin(TARGET ${PROJECT_NAME}
    CLAP_ID "com.dimethoxy.disflux"
    CLAP_FEATURES audio-effect filter equalizer)
