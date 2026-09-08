# **SL Link - Extended Controller API**

The **SL Link - Extended Controller API** documents the protocol used to manage connection, communication, and display interaction between the [SL MK2](https://www.studiologic-music.com/products/slmk2/) and a remote device.

Communication is based on MIDI System Exclusive (SysEx) messages, providing a standards-based transport for proprietary SL Link functionality. On macOS, the SLMK2 sends and receives SL Link messages through the port named `SL LINK`. On Windows, the corresponding ports are `MIDIN3` and `MIDIOUT3`.

## Overview

This documentation describes the SL Link message model and the message families used to exchange data with the device, including identification, system, display, and hardware I/O communication.

It is intended for developers building host software, integrations, or tools that need to communicate with the SLMK2 at the protocol level.

## Documentation structure

- [Basics](basics.md)
- [Message Structure](message-structure.md)
- [Identification Messages](identification-messages.md)
- [System Messages](system-messages.md)
- [Display Messages](display-messages.md)
- [Hardware I/O](hardware-io.md)
- [Appendix A](appendix-a.md)

## Getting started

A good reading order is:

1. Start with [Basics](basics.md) for the overall protocol context.
2. Continue with [Message Structure](message-structure.md) to understand packet layout and encoding.
3. Read the individual message categories as needed:
   - [Identification Messages](identification-messages.md)
   - [System Messages](system-messages.md)
   - [Display Messages](display-messages.md)
   - [Hardware I/O](hardware-io.md)
4. Use [Appendix A](appendix-a.md) for supporting reference material.

### Notes

- This documentation focuses on SL Link protocol behavior and message definitions.
- MIDI transport uses SysEx messages throughout the protocol.
- Port naming differs between macOS and Windows, so software should account for platform-specific MIDI device names.

## Additional Resources
This reference section includes essential links for getting started and diving deeper into Studiologic products, JUCE framework, and MIDI technology.

**A. Audio Developer Conference (ADC)**  
Annual conference focused on audio development, often featuring JUCE-related content and talks.

**B. GitHub Repositories** 

- [JUCE on GitHub](https://github.com/juce-framework/JUCE): Official JUCE repository with source code and contributions from the community.  
- [MIDI Projects on GitHub](https://github.com/topics/midi): Explore various MIDI-related projects on GitHub.

**C. Books and Guides**

- [Designing Audio Effect Plug-Ins in C++](https://www.amazon.com/Designing-Audio-Effect-Plug-Ins-Software/dp/1138499710): A book by Will Pirkle that provides an in-depth look at audio effect plugin design using C++ and frameworks like JUCE.
- [MIDI Power!: The Comprehensive Guide](https://www.amazon.com/MIDI-Power-Comprehensive-Robert-Gu%C3%A9rin/dp/1598630842): A comprehensive guide to understanding and using MIDI technology.