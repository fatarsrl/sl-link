# Identification Messages

## Identification mechanics
Identification messages represent the first interaction between the Device and the SLMK2.  
The Device is responsible for the first communication with the keyboard, and must do this via a ***Identification Request***.  

After receiving an ***Identification Approved*** message from the SLMK2 the Device is listed in the available apps menu (reached by pressing the APP button on the Keyboard).  

From this moment on the Device must send a ***System Device Notification*** message continuously at a rate of less than one every 5 seconds (e.g. one every 3/4 seconds).  
If the SLMK2 does not receive this message for more than 5 seconds the Device will not be listed in the App Menu anymore.  
In this case, in order to be listed again among the available apps, the Device must send a new ***Identification Request*** message.

#### About Device ID
One important dynamic of Identification Messages is the one regarding the DeviceID.  

With the ***Identification Request*** message, the device must provide an unique, **non zero**, 14-bit ID in order to gain access to the App List via the two DeviceID bytes.  
The suggested method to do this is to generate two random 7-bit numbers assigned to the two dedicated bytes. 
In this way, in case the Device is a software, different instances will automatically gain different IDs.  
Every number is accepted with the exception of the couples (`0x00`, `0x00`) and (`0x01`, `0x00`)  that are reserved by Studiologic.  


If a DeviceID is already listed in the App Menu, or a reserved Device ID is sent (e.g. `0x00`, `0x00`), an ***Identification Rejected*** message with a RSN Byte of `0x00` is sent as a response to the ***Identification Request*** message (see [***Identification Rejected Message***](#identification-rejected-message)).  
In this case the Device must generate a new ID and try the Identification again.

Another case of rejection is when the App List is full, and an ***Identification Rejected*** message with a RSN Byte of `0x01` is sent from the SLMK2 to the Device.  
At the moment the list is capped at 10 for the SLMK2, but this number can vary in future firmware updates.

In all other cases an ***Identification Approved*** message is sent, and from this moment on the two 7-bit bytes provided, will identify all the messages from and to the Device.


## Structure

Every Identification Message is identified by the ItemType byte set to `0x7F` and each one is distinguished by the **Function** byte.

| `F0 00 20 1A 16 ID#1 ID#2` | `0x7F` | Function | *...message...* | `F7` |
|:---:|:---:|---|---|---|
| Header | ItemType | | | |

Each Identification Message can flow only in one direction, and comprehensive list of all the messages is listed here:

| Identification Message | Function byte | Direction |
|:---|:---:|:---:|
| Identification Request | `0x00` | Device → SL |
| Identification Approved | `0x01` | Device ← SL |
| Identification Rejected | `0x02` | Device ← SL |
| Identification Query | `0x03` | Device ↔ SL |

### Identification Request Message
| `F0 00 20 1A 16` | `ID#1 ID#2` | `0x7F`| `0x00` | S(1)... S(N) | `F7` |
|:---:|:---:|:---:|:---:|:---:|:---:|
| Header | DeviceID | ItemType | Function | Identification String |  |

With this message the remote device requests SLMK2 to include it in the list of available Apps accessible through the ***APP*** button.

If space is available in the list and the DeviceID is not already taken, an ***Identification Approved*** message is sent as a response, otherwise an ***Identification Rejected*** message will be received by the Device.

This message must contain a name string, represented by S(N) ASCII bytes, along with its string terminator (`\0` = `0x00`).  
This string will identify the sending device in the above mentioned list, and has a maximum length of 32 bytes.  
As we will see later, the SLMK2 fonts only contain ASCII characters from `0x20` to `0x80` included, and every other character sent will be replaced by a space.

### Identification Approved Message

If the given DeviceID is not already taken by some other Device, and there is still space available in the App List, the following message is sent by the SLMK2 as a responses to the ***Identification Request*** message:

<table>
<thead>
<tr>
<th align="center"><code>F0 00 20 1A 16</code></th>
<th align="center"><code>ID#1 ID#2</code></th>
<th align="center"><code>0x7F</code></th>
<th align="center"><code>0x01</code></th>
<th align="center"><code>F7</code></th>
</tr>
</thead>
<tbody><tr>
<td align="center">Header</td>
<td align="center">DeviceID</td>
<td align="center">ItemType</td>
<td align="center">Function</td>
<td></td>
</tr>
</tbody>
</table>

Upon receiving this value, the Device should proceed to send the ***System Device Acknowledge*** message at a rate of less than one every 5 seconds.

### Identification Rejected Message

If no more Apps can be listed in the appropriate menu, or the given DeviceID is already present in the App menu, the following message is sent by the SLMK2:

| `F0 00 20 1A 16` | `ID#1 ID#2` | `0x7F` | `0x02` | `RSN` | `F7` | 
|:---:|:---:|:---:|:---:|:---:|:---:|
| Header | DeviceID | ItemType | Function | Rejection reason| |

The `RSN` byte contains the reason of the rejection:

| `RSN` | Reason |
|:---:|:---:|
| `0x00` | DeviceID already taken or reserved |
| `0x01` | No more space in the list |

In case RSN = `0x00`, the device must generate a new DeviceID and try another ***Identification Request***.

In case RSN = `0x01`, the device can try a new ***Identification Request*** later.   
Note that the SLMK2 will not notify the pending Devices when a slot is freed from the list, and the Device is responsible of checking from time to time.

### Identification Query Message

This message queries the SL to determine whether the DeviceID provided is already identified on the SL.
The Device is responsible for knowing whether the supplied ID corresponds to (or not) to a previously accepted Identification Request.

The request is structured as follows:

| `F0 00 20 1A 16` | `ID#1 ID#2` | `0x7F` | `0x03` |`F7` | 
|:---:|:---:|:---:|:---:|:---:|
| Header | DeviceID | ItemType | Function | |

The SL than respond with this message, containing the requested information

| `F0 00 20 1A 16` | `ID#1 ID#2` | `0x7F` | `0x03` | `IDF` | `F7` | 
|:---:|:---:|:---:|:---:|:---:|:---:|
| Header | DeviceID | ItemType | Function | Rejection reason| |

The `IDF` byte indicate the identification status:

| `IDF` | Reason |
|:---:|:---:|
| `0x00` | DeviceID is not identified on the machine |
| `0x01` | DeviceID is already identified |

[Back to index](index.md)
<!-- [Back to index](../README.md) -->
