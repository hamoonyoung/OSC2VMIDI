# OSC2VMIDI 0.01a

OSC2VMIDI is a prototype command-line tool that bridges the gap between OSC (Open Sound Control) data and MIDI. It receives OSC data over Ethernet or Wi-Fi and translates it into MIDI messages, which are then sent to various software applications and DAWs via a virtual MIDI device. Leveraging the powerful JUCE framework, OSC2VMIDI offers a reliable and efficient way to enhance your music production and live performance setups by integrating OSC and MIDI technologies.

It was developed for my students who would like to use MIDI controllers but do not have the money to buy this equipment. Most students have a smartphone, so they can download free or affordable OSC-based controller apps such as Syntien (https://blackislandaudio.com/products/syntien) or TouchOSC (https://hexler.net/touchosc). Then, OSC2VMIDI can transfer data between these apps and DAWs or Max/MSP.

## Key Features:
- OSC to MIDI Conversion: Converts incoming OSC messages to MIDI messages.
- Virtual MIDI Device: Creates a virtual MIDI device to interface with other software and DAWs.
- Network Connectivity: Supports Ethernet and Wi-Fi for receiving OSC data.
- JUCE Framework: Built with the JUCE framework for high performance and flexibility.
- Command-Line Interface: Simple and efficient command-line interface for easy use and automation.

OSC2VMIDI has been developed and tested only on macOS Sonoma 14.5.
