# SL Link - Extended Controller API

The SL Link - Extended Controller API provides comprehensive functionality for managing the connection, communication, and display interaction between SLMK2 and the remote device, ensuring optimal performance and user experience.

The communication protocol employed is MIDI, leveraging the transmission and reception of 'System Exclusive' messages. This approach ensures compatibility and flexibility in data exchange between devices, adhering to MIDI standards while accommodating proprietary functionalities specific to the SLMK2 and the connected remote device.
<br>The SLMK2 receives and sends SL-Link messages on the port called “SL LINK” on macOS and MIDIN3/MIDIOUT3 on Windows.

- [Basics](docs/basics.md)
- [SL Link message structure](docs/message-structure.md)
- [Identification Messages](docs/identification-messages.md)
- [System Messages](docs/system-messages.md)
- [Display Messages](docs/display-messages.md)
- [Hardware I/O](docs/hardware-io.md)

---

- [Appendix A](docs/appendix-a.md)
- [JUCE MIDI example project](./examples/juce_midi_example_project/readme.md)

## References

**STUDIOLOGIC**

- [Studiologic Official website](https://www.studiologic-music.com) for Studiologic products including manuals, software updates, and support.

**JUCE**  

- [JUCE Official Website](https://juce.com/) for JUCE, a widely used C++ framework for developing cross-platform audio applications.

**MIDI Manufacturers Association (MMA)**  

- [MIDI.org](https://www.midi.org/): The official site of the MIDI Manufacturers Association with resources, specifications, and news about MIDI technology.

### Additional Resources

**Audio Developer Conference (ADC)**  
Annual conference focused on audio development, often featuring JUCE-related content and talks.

**GitHub Repositories**  
- [JUCE on GitHub](https://github.com/juce-framework/JUCE): Official JUCE repository with source code and contributions from the community.  
- [MIDI Projects on GitHub](https://github.com/topics/midi): Explore various MIDI-related projects on GitHub.

**Books and Guides**
- [Designing Audio Effect Plug-Ins in C++](https://a.co/d/04GYhWIC): A book by Will Pirkle that provides an in-depth look at audio effect plugin design using C++ and frameworks like JUCE.
- [MIDI Power!: The Comprehensive Guide](https://a.co/d/00SjvSUf): A comprehensive guide to understanding and using MIDI technology.

This reference section includes essential links for getting started and diving deeper into Studiologic products, JUCE framework, and MIDI technology.
