# SL Link message structure
The SL Link provides a full range protocol to give comprehensive control of the keyboard to the developer.  
This is achieved by implementing specific Sysex messages for streaming the I/O from and to the keyboard.  
The messages are both sent and received by the SLMK2.

## General SL-Link Message Structure

### Header and Stop

All the SL-Link messages are created appending different commands to a standard Sysex message header
with the standard 3-byte manufacturers' ID number, and with the addition of one byte for the ProductID and two for the DeviceID, required for identification of the Device currently communicating with the SLMK2.

As previiously said the messages must end with an `F7` byte.

[//]: # (| `F0` |`00 20 1A` | `16` | ID#1 | ID#2 | *...message...* | `F7` | )
[//]: # (|:---:|:---:|:---:|:---:|:---:|:---:|:---:|)
[//]: # (| Sysex start | FATAR manufacturer ID | SL product ID | DeviceID #1 | DeviceID #2 | | Sysex stop | )
[//]: # (|| <td colspan=5>Header |||)

<table>
	<thead>
		<tr>
			<th align="center"><code>F0</code></th>
			<th align="center"><code>00 20 1A</code></th>
			<th align="center"><code>16</code></th>
			<th align="center">ID#1</th>
			<th align="center">ID#2</th>
			<th align="center"><em>...message...</em></th>
			<th align="center"><code>F7</code></th>
		</tr>
	</thead>
	<tbody>
		<tr>
			<td align="center">Sysex start</td>
			<td align="center">FATAR manufacturer ID</td>
			<td align="center">SLMK2 product ID</td>
			<td align="center">DeviceID #1</td>
			<td align="center">DeviceID #2</td>
			<td align="center"></td>
			<td align="center">Sysex stop</td>
		</tr>
		<tr>
			<td align="center"></td>
			<td align="center" colspan=4>Header</td>
			<td align="center"></td>
			<td align="center"></td>
		</tr>
	</tbody>
</table>

### DeviceID

To avoid conflicts between different application streams of messages, each device must generate a 14-bit unique identification number.
<br>This identification number, called DeviceID, must be packed in two 7-bit bytes, due to the costrains of the MIDI protocol.

Moreover, the identification number must be approved by the SLMK2 via an exchange of [Identification Messages](./identification-messages.md), and, if this happens, these two bytes will identify the Device for all the course of the session, and will be included in the header of every message exchanged by the Device and the SLMK2.

This topic is fully described in the [Identification Messages](./identification-messages.md) section.

### Item Type

The byte that follows the DeviceID is called `ItemType`, and identifies the type of message the current Sysex represents.  
The SL-Link messages are grouped in six different categories.  
Such categories and their specific messages will be addressed in the following sections.

| `F0 00 20 1A 16 ID#1 ID#2` | ItemType | *...message...* | `F7` |
|:---:|:---:|:---:|:---:|
| Header | | | |			

### List of message types

The SL-Link messages are grouped in six different categories, four of which are further grouped into one macro-category.  
We present a brief list of these categories below; each one will be addressed in detail in the following sections



| ItemType | Corresponding message |
|:---:|:---|
| `0x7F` | Identification Messages |
| `0x00` | System Messages |
| `0x04` | Display Messages |
| `0x01` | **Button messages**[^1] |
| `0x02` | **LED messages**[^1] |
| `0x03` | **Encoder Messages**[^1] |
| `0x05` | **RGB LED messages**[^1] |
| `0x06` | **Pedal message**[^1] |

[^1]: Grouped in the Hardware I/O messages macro-category.

The three resulting types of messages that will be discussed separately.

1. **Identification Messages ( ItemType = `0x7F` )**
Two-way acknowledged messages used to notify the SL-mk2 of a newly present device.

2. **System Messages ( ItemType = `0x00` )**  
Messages that handle connection state: acknowledge, login, logout, standby, restart, and device icon management.

3. **Display Messages ( ItemType = `0x04` )**  
One-way messages from the device to the SL-mk2 for writing to the LCD using text, rectangles, predefined bitmaps, and the device icon.

4. **Hardware I/O ( ItemType = `0x01`, `0x02`, `0x03`, `0x05`, `0x06`)**  
Messages carrying user-interaction events from encoders, buttons, LEDs, RGB LEDs, pedals, and for retrieving or storing hardware settings.

[Back to index](index.md)
<!-- [Back to index](../README.md) -->
