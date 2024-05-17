//
// Created by Moon Ha on 5/15/24.
//

#include "VirtualMidiDevice.h"

VirtualMIDIDevice::VirtualMIDIDevice(): midiInputDeviceName((std::string &) " From Virtual Midi Device"), midiOutputDeviceName ((std::string &) " To Virtual Midi Device")  {

}

VirtualMIDIDevice::VirtualMIDIDevice(std::string & inputName, std::string & outputName) : midiInputDeviceName(inputName), midiOutputDeviceName (outputName)
{
    if( virtualMidiInputDevice == nullptr ) {
        std::cout << "couldn't create virtual midi IN port for VirtualMIDIDevice: virtualMidiInputDevice\n";
        exit(1);
    } else {
        //std::cout << "VirtualMIDIDevice: virtualMidiInputDevice was created.\n";
        virtualMidiInputDevice->start();
        //std::cout << "VirtualMIDIDevice: virtualMidiInputDevice started.\n";
    }
    if( virtualMidiOutputDevice == nullptr ) {
        std::cout << "couldn't create virtual midi OUT port for VirtualMIDIDevice: virtualMidiOutputDevice\n";
        exit(2);
    } else {
        //std::cout << "VirtualMIDIDevice: virtualMidiOutputDevice was created.\n";
    }
}

VirtualMIDIDevice::~VirtualMIDIDevice()
{
    virtualMidiInputDevice->stop();
    //std::cout << "VirtualMIDIDevice: virtualMidiInputDevice stopped.\n";
}




VirtualMIDIDevice::MidiMessageCallback::MidiMessageCallback() {

}

VirtualMIDIDevice::MidiMessageCallback::~MidiMessageCallback() {

}

void
VirtualMIDIDevice::MidiMessageCallback::handleIncomingMidiMessage(juce::MidiInput *source, const MidiMessage &message) {
//    source->getName();
//    message.getChannel();
//    message.getNoteNumber();
//    static_cast<int>(message.getVelocity());
    callback(source->getName().toStdString(), static_cast<int>(message.getChannel()), static_cast<int>(message.getNoteNumber()), static_cast<int>(message.getVelocity()));

}

void VirtualMIDIDevice::MidiMessageCallback::setCallback(std::function<void(std::string, int, int, int)> callback) {
    this->callback = callback;
}

VirtualMIDIDevice::MidiSender::MidiSender() {
    // Open a MIDI output device
    auto midiOutputs = juce::MidiOutput::getAvailableDevices();
    if (midiOutputs.size() > 0)
        midiOutput = juce::MidiOutput::openDevice(midiOutputs[0].identifier);
}

VirtualMIDIDevice::MidiSender::~MidiSender() {
// Close the MIDI output device
    if (midiOutput != nullptr)
        midiOutput.reset();
}

void VirtualMIDIDevice::MidiSender::sendMidiCCMessage(int channel, int controllerNumber, int value) {
    if (midiOutput != nullptr)
        midiOutput->sendMessageNow(juce::MidiMessage::controllerEvent(channel, controllerNumber, value));
}

void VirtualMIDIDevice::MidiSender::sendMidiNoteMessage(int channel, int noteNumner, int velocity) {
    if (midiOutput != nullptr){
        juce::MidiMessage noteOn (juce::MidiMessage::noteOn (channel, noteNumner, (juce::uint8)velocity));
        noteOn.setTimeStamp (juce::Time::getMillisecondCounterHiRes() * 0.001);

        juce::MidiMessage noteOff (juce::MidiMessage::noteOff (channel, noteNumner, (juce::uint8)velocity));
        noteOff.setTimeStamp (juce::Time::getMillisecondCounterHiRes() * 0.001 + 0.5); // 0.5 second later

        midiOutput->sendMessageNow (noteOn);
    }
}