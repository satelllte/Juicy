# The first line of any CMake project should be a call to `cmake_minimum_required`, which checks
# that the installed CMake will be able to understand the following CMakeLists, and ensures that
# CMake's behaviour is compatible with the named version. This is a standard CMake command, so more
# information can be found in the CMake docs.
# - 3.24.1 is bundled in Visual Studio 2022 v17.4
# - 3.24.1 is also bundled in CLion as of 2023
cmake_minimum_required(VERSION 3.24.1)

# Project name.
# This cannot have spaces!
set(PROJECT_NAME "Juicy")

# Product name.
# This can have spaces.
set(PRODUCT_NAME "Juicy")

# Plugin formats to build.
# Valid options: AAX Unity VST AU AUv3 Standalone
set(FORMATS AU VST3 AUv3)

# This must be set before the project() call
# see: https://cmake.org/cmake/help/latest/variable/CMAKE_OSX_DEPLOYMENT_TARGET.html
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Support MacOS down to High Sierra")

# Reads VERSION file and puts its value into CURRENT_VERSION variable.
# Be sure the file has no newlines!
file(STRINGS VERSION CURRENT_VERSION)

# Target name. 
# For simplicity, it takes the project name.
project(${PROJECT_NAME} VERSION ${CURRENT_VERSION})

# By default we don't want Xcode schemes to be made for modules, etc
set(CMAKE_XCODE_GENERATE_SCHEME OFF)

# Building universal binaries on MacOS increases build time.
# We do this only for production builds.
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message("Building for Apple Silicon and x86_64")
    set(CMAKE_OSX_ARCHITECTURES arm64 x86_64)
endif ()

# Adds all the module sources so they appear correctly in the IDE.
# Must be set before JUCE is added as a sub-dir (or any targets are made).
# https://github.com/juce-framework/JUCE/commit/6b1b4cf7f6b1008db44411f2c8887d71a3348889
set_property(GLOBAL PROPERTY USE_FOLDERS YES)

# Create a /Modules directory in the IDE with the JUCE Module code
option(JUCE_ENABLE_MODULE_SOURCE_GROUPS "Show all module sources in IDE projects" ON)

# JUCE is setup as a submodule in the /JUCE folder.
# Locally, you'll need to run `git submodule update --init --recursive` once
# and `git submodule update --remote --merge` to keep it up to date.
# On Github Actions, it's managed by actions/checkout.
add_subdirectory(JUCE)

# Check out https://github.com/juce-framework/JUCE/blob/master/docs/CMake%20API.md for full config
juce_add_plugin("${PROJECT_NAME}"
	# VERSION ...                               # Set this if the plugin version is different to the project version
	# ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
	# ICON_SMALL ...
    COMPANY_NAME "satelllte"
	BUNDLE_ID com.satelllte.juicy
	IS_SYNTH FALSE                              # Is this a synth or an effect?
	NEEDS_MIDI_INPUT FALSE                      # Does the plugin need midi input?
	NEEDS_MIDI_OUTPUT FALSE                     # Does the plugin need midi output?
	IS_MIDI_EFFECT FALSE                        # Is this plugin a MIDI effect?
	EDITOR_WANTS_KEYBOARD_FOCUS FALSE           # Does the editor need keyboard focus?
	COPY_PLUGIN_AFTER_BUILD TRUE                # On MacOS, plugin will be copied to /Users/you/Library/Audio/Plug-Ins/
	PLUGIN_MANUFACTURER_CODE "STLT"             # This has to be one uppercase, rest lower for AU formats
	PLUGIN_CODE "JJCY"                          # A unique four-character plugin id with at least one upper-case character
	FORMATS "${FORMATS}"
	PRODUCT_NAME "${PRODUCT_NAME}")             # The name of the final executable, which can differ from the target name

# Use C++20
target_compile_features("${PROJECT_NAME}" PRIVATE cxx_std_20)

# Manually list all .h and .cpp files for the plugin
set(SourceFiles
    Source/PluginEditor.h
	Source/PluginProcessor.h
	Source/PluginEditor.cpp
	Source/PluginProcessor.cpp)
target_sources("${PROJECT_NAME}" PRIVATE ${SourceFiles})

# We don't want our source buried in extra nested folders.
set_target_properties("${PROJECT_NAME}" PROPERTIES FOLDER "")

# Fix Xcode source tree look.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Source PREFIX "" FILES ${SourceFiles})

# Clean up folder organization on Xcode.
# It tucks the Plugin varieties into a "Targets" folder and generate an Xcode Scheme manually
# Xcode scheme generation is turned off globally to limit noise from other targets
# The non-hacky way of doing this is via the global PREDEFINED_TARGETS_FOLDER property
# However that doesn't seem to be working in Xcode
# Not all plugin types (AU, VST) available on each build type (Win, MacOS, Linux)
foreach (target ${FORMATS} "All")
    if (TARGET ${PROJECT_NAME}_${target})
        set_target_properties(${PROJECT_NAME}_${target} PROPERTIES
            # Tuck the actual plugin targets into a folder where they won't bother us
            FOLDER "Targets"

            # MacOS only: Sets the default executable that Xcode will open on build
            # For this exact path to to work, manually build the AudioPluginHost.xcodeproj in the JUCE subdir
            XCODE_SCHEME_EXECUTABLE "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/extras/AudioPluginHost/Builds/MacOSX/build/Debug/AudioPluginHost.app"

            # Let us build the target in Xcode
            XCODE_GENERATE_SCHEME ON)
    endif ()
endforeach ()

target_compile_definitions("${PROJECT_NAME}"
    PUBLIC
    # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
    JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
    JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries("${PROJECT_NAME}"
    PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)
