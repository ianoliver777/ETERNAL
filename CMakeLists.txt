cmake_minimum_required(VERSION 3.22)
project(ETERNAL VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
include(FetchContent)
FetchContent_Declare(juce GIT_REPOSITORY https://github.com/juce-framework/JUCE.git GIT_TAG 8.0.10 GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(juce)
if(APPLE)
  set(PLUGIN_FORMATS VST3 AU Standalone)
else()
  set(PLUGIN_FORMATS VST3 Standalone)
endif()
juce_add_plugin(ETERNAL COMPANY_NAME "Triple Seven Audio" BUNDLE_ID "com.triplesevenaudio.eternal"
  PLUGIN_MANUFACTURER_CODE Ts7a PLUGIN_CODE Et7l FORMATS ${PLUGIN_FORMATS}
  PRODUCT_NAME "ETERNAL" IS_SYNTH FALSE NEEDS_MIDI_INPUT FALSE NEEDS_MIDI_OUTPUT FALSE
  COPY_PLUGIN_AFTER_BUILD FALSE)
target_sources(ETERNAL PRIVATE Source/PluginProcessor.cpp Source/PluginProcessor.h Source/PluginEditor.cpp Source/PluginEditor.h)
target_compile_definitions(ETERNAL PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(ETERNAL PRIVATE juce::juce_audio_utils juce::juce_dsp
  PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_lto_flags juce::juce_recommended_warning_flags)
