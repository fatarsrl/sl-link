# Basics

## What are Sysex Messages?

MIDI System Exclusive messages, commonly abbreviated as Sysex, are a special category of MIDI message designed for transmitting proprietary data. These messages allow communication between devices from the same manufacturer or of the same model.

Unlike standard MIDI messages, Sysex messages are not universal. Their format and interpretation are entirely determined by the device manufacturer.

## Structure of Sysex Messages

Sysex messages consist of a series of bytes that always start with a specific status byte (`0xF0`) and always end with an end-of-message byte (`0xF7`). The general structure is as follows:

1. **Start status byte**: `0xF0` (240 in decimal).

2. **Manufacturer identifier**: One or three bytes that identify the manufacturer of the device.  
For example, `0x41` for Roland, `0x7E` and `0x7F` for universal messages.  
The FATAR (Studiologic) identifier is `0x00` (byte 0), `0x20` (byte 1) and `0x1A` (byte 2)

3. **Specific data**: A sequence of bytes containing manufacturer-specific data. These bytes vary depending on the manufacturer and the type of message.

4. **End-of-message byte**: `0xF7` (247 in decimal).

It is important to note that the MIDI standard requires the Most Significant Bit (MSB) to be zero for all data bytes. The only exceptions are MIDI Status bytes and the End-of-Message byte.

We will take this rule for granted throughout this guide. Therefore, whenever we refer to a "byte," we are always referring to 7 bits of data.

## Example of a Sysex Message

Consider the following Sysex message: `F0 00 20 1A 16 15 E3 04 01 00 F7`.
<br>This can be broken down as follows:

| `F0` | `00` : `20` : `1A` | `16` : `15` : `E3` : `04` : `01` : `00` | `F7` |

- `F0`: 		Start of the Sysex message;
- `00 20 1A`: 	Manufacturer identifier (FATAR);
- `16 15 E3 04 01 00`: 	Specific data of the message;
- `F7`: 		End of the Sysex message.

This example identifies a specific SL-Link message that can be decoded using this documentation.

[^1]: For a detailed Sysex and MIDI reference, see [MIDI 1.0 Detailed Specification](https://midi.org/midi-1-0-detailed-specification).

[Back to index](index.md)
<!-- [Back to index](../README.md) -->