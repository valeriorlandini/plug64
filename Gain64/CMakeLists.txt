cmake_minimum_required(VERSION 3.19)

set(BaseTargetName Gain64)
set(PluginName "Gain64")

juce_add_plugin("${BaseTargetName}"
        COMPANY_NAME "Valerio Orlandini"
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT FALSE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE
        COPY_PLUGIN_AFTER_BUILD FALSE
        PLUGIN_MANUFACTURER_CODE Vorl
        PLUGIN_CODE Ga64
        FORMATS AU VST3 LV2 Standalone
        PRODUCT_NAME "${PluginName}"
	BUNDLE_ID "com.valeriorlandini.gain64"
	LV2URI "http://www.valeriorlandini.com/plugins/gain64")

target_sources(${BaseTargetName} PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        ${CMAKE_SOURCE_DIR}/Shared/CustomLookAndFeel.cpp)

target_compile_definitions(${BaseTargetName}
        PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_include_directories(${BaseTargetName} PRIVATE ${CMAKE_SOURCE_DIR}/Shared)

target_link_libraries(${BaseTargetName} PRIVATE
        BinaryData
	juce_dsp
	juce_audio_utils
	juce_audio_devices
        juce_recommended_config_flags
        juce_recommended_lto_flags
        juce_recommended_warning_flags)
