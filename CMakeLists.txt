cmake_minimum_required(VERSION 3.27)
project(OSC2VMIDI VERSION 0.0.1)
set(CMAKE_CXX_STANDARD 17)

#add_executable(OSC2VMIDI main.cpp)
#find_package(JUCE CONFIG REQUIRED)
add_subdirectory(JUCE)

juce_add_console_app(OSC2VMIDI
        PRODUCT_NAME "OSC2VMIDI")

juce_generate_juce_header(OSC2VMIDI)

target_sources(OSC2VMIDI
        PRIVATE
        main.cpp
        VirtualMidiDevice.cpp
        VirtualMidiDevice.h
        )

add_library(UDPServer UDPServer.cpp UDPServer.h)
#add_library(VirtualMidiDevice VirtualMidiDevice.cpp VirtualMidiDevice.h)

target_compile_definitions(OSC2VMIDI
        PRIVATE
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_console_app` call
        JUCE_USE_CURL=0)    # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_console_app` call

target_link_libraries(OSC2VMIDI
        PRIVATE
        juce::juce_core
        juce::juce_audio_devices
        UDPServer
       # VirtualMidiDevice
        ncurses
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)