//
// Created by Moon Ha on 5/15/24.
//
#ifndef OSC2VMIDI_VIRTUALMIDIDEVICE_H
#define OSC2VMIDI_VIRTUALMIDIDEVICE_H

#include <iostream>
#include <juce_core/juce_core.h>
#include <JuceHeader.h>

class VirtualMIDIDevice {
public:
    VirtualMIDIDevice ();
    VirtualMIDIDevice( std::string & inputName,  std::string & outputName);

    ~VirtualMIDIDevice();
    class MidiMessageCallback : public juce::MidiInputCallback{
    public:
        MidiMessageCallback();
        ~MidiMessageCallback() override;
        void handleIncomingMidiMessage (juce::MidiInput* source, const juce::MidiMessage& message) override;

        std::function<void(std::string, int, int, int)> callback;
        void setCallback(std::function<void(std::string, int, int, int)> callback);
    };

    class MidiSender{
    public:
        MidiSender();
        ~MidiSender();
        // Send a MIDI CC message
        void sendMidiCCMessage(int channel, int controllerNumber, int value);
        void sendMidiNoteMessage(int channel, int noteNumber, int velocity);

    private:
        std::unique_ptr<juce::MidiOutput> midiOutput;

    };

    MidiMessageCallback midiInputCallback;
    MidiSender midiSender;
private:
    //juce::AudioDeviceManager deviceManager;
    std::string &  midiInputDeviceName;
    std::string &  midiOutputDeviceName;

    std::unique_ptr< MidiInput > virtualMidiInputDevice = juce::MidiInput::createNewDevice(midiInputDeviceName, & midiInputCallback);
    std::unique_ptr< MidiOutput > virtualMidiOutputDevice = juce::MidiOutput::createNewDevice(midiOutputDeviceName);

};
#endif //OSC2VMIDI_VIRTUALMIDIDEVICE_H
