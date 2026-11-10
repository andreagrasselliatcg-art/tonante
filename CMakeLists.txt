cmake_minimum_required(VERSION 3.22)

set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS version")
if(APPLE AND NOT CMAKE_OSX_ARCHITECTURES)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
endif()

project(Tonante VERSION 1.0.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TONANTE_JUCE_DIR "" CACHE PATH "Path to a local JUCE checkout (optional)")
if(TONANTE_JUCE_DIR)
    add_subdirectory(${TONANTE_JUCE_DIR} JUCE)
else()
    include(FetchContent)
    FetchContent_Declare(JUCE GIT_REPOSITORY https://github.com/juce-framework/JUCE.git GIT_TAG 8.0.4 GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(JUCE)
endif()

option(TONANTE_COPY_AFTER_BUILD "Copy plugins to the user plug-in folders after build" ON)
if(APPLE)
    set(TONANTE_FORMATS AU VST3 Standalone)
else()
    set(TONANTE_FORMATS VST3 Standalone)
endif()

juce_add_plugin(Tonante
    PRODUCT_NAME                "Tonante"
    COMPANY_NAME                "Tune Talks"
    BUNDLE_ID                   com.tunetalks.tonante
    PLUGIN_MANUFACTURER_CODE    TnTk
    PLUGIN_CODE                 Tnnt
    DESCRIPTION                 "Three oscillator monosynth"
    FORMATS                     ${TONANTE_FORMATS}
    IS_SYNTH                    TRUE
    NEEDS_MIDI_INPUT            TRUE
    NEEDS_MIDI_OUTPUT           FALSE
    IS_MIDI_EFFECT              FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD     ${TONANTE_COPY_AFTER_BUILD}
    AU_MAIN_TYPE                kAudioUnitType_MusicDevice
    VST3_CATEGORIES             Instrument Synth Mono)

target_sources(Tonante PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp)
target_include_directories(Tonante PRIVATE Source)

juce_add_binary_data(TonanteFonts
    HEADER_NAME BinaryData.h
    NAMESPACE   BinaryData
    SOURCES     Source/fonts/SpaceGrotesk-Regular.ttf Source/fonts/SpaceGrotesk-Bold.ttf)
set_target_properties(TonanteFonts PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_compile_definitions(Tonante PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(Tonante
    PRIVATE TonanteFonts juce::juce_audio_utils
    PUBLIC  juce::juce_recommended_config_flags juce::juce_recommended_lto_flags juce::juce_recommended_warning_flags)

option(TONANTE_TESTS "Build synth unit test" OFF)
if(TONANTE_TESTS)
    add_executable(tonante_synth_test tests/test_synth.cpp)
    target_include_directories(tonante_synth_test PRIVATE Source)
    if(MSVC)
        target_compile_definitions(tonante_synth_test PRIVATE _USE_MATH_DEFINES)
    endif()
endif()
