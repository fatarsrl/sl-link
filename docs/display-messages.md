# Display Messages

As above mentioned, the Display Messages are used to manage the LCD output of the SL.  
This set is the most complicated of the A.P.I. and must be treated and understood carefully.

Furthermore the LCD management is particularly resource demanding in terms of computation and I/O machine time, so particular care must be taken when constructing the user interface.

## About the Coordinates

Before proceeding with a detailed explanation of each Display Message, it is necessary to clarify the functioning of the X and Y coordinate mechanism.

The SLMK2 is equipped with a 320x240 (X,Y) LCD display, with the (0,0) coordinate placed in the upper left corner, and the (319,239) in the lower right corner.

We already mentioned that the MIDI protocol requires the Most Significant Bit to be set to zero, leaving 7 bits of bandwidth for information.  
This implies that two bytes per coordinate (four in total) are needed in order to properly communicate one point on the screen.

The two bytes are organized in MSB and LSB bytes, with the 7 Least Significant Bit stored in the LSB byte and the remaining seven bit stored in the MSB byte (though only two are actually needed).  
This concept actually applies to every chunk of data that exceeds the 7-bit limit of the MIDI Data Bytes, as we will see further ahead.

To further clarify the concept some example are given:

| | 16-bit  coordinate Value (dec and bin) | 8-bit MSB | 8-bit LSB |
|:---:|:---:|:---:|:---:|
| X | 84 `0b0000 0000 0101 0100` | 0 `0b0000 0000` | 84 `0b0101 0100` |
| Y | 156 `0b0000 0000 1001 1100` | 1 `0b0000 0001` | 28 `0b0001 1100` |
| X | 293 `0b0000 0001 0010 0101` | 2 `0b0000 0010` | 5 `0b0010 0101` |
| Y | 31 `0b0000 0000 0001 1111` | 0 `0b0000 0000` | 31 `0b0001 1111` |

The code below can be integrated to convert the 16-bit value for the X or Y axes (and for other data exceeding 7-bit of range).  
The return value is a structure (tuple) with two fields for the MSB and LSB values.  
This principle will become clearer when the examples are presented.

```
struct MsbLsb
{
    MsbLsb(uint8_t m, uint8_t l) : msb(m), lsb(l)     {}
    uint8_t msb = 0;
    uint8_t lsb = 0;
};

MsbLsb SysexManagerNew::get7BitMsbLsb(uint16_t val) {
    uint8_t msb = val >> 0x07;
    uint8_t lsb = val & 0x7F;
    return MsbLsb(msb, lsb);
}
```

## About the Colors

The most common standard for color coding is the 8-bit per channel, 32-bit RGBA.  
The SLMK2 does not support the alpha channel, leaving us with 24-bit of data to handle, 8-bit per color.

As we pointed out above, the MIDI standard requires the use of 7-bits per byte, forcing us to manage the color coding differently.

That said, all the messages that involve colors (all the Display ones but also the RGB LED message), accept 7-bit of data per channel, each in its own Data Byte.  
This implies a loss in color depth precision, but given the color resolution of the SLMK2 LCD display, no difference will be noticed.

The conversion between a 24-bit RGB value can be achieved by keeping the 7-th most significant bit of each channel (in other words shifting one bit right).  
As an example we show a C function that perform a step-by-step conversion from 24-bit RGB to our 7-bit per channel format:

```
struct Color21
{
    uint8_t red : 7;
    uint8_t green : 7;
    uint8_t blue : 7;
};

Color21 RGB24_to_21(uint32_t rgb24)
{
	uint8_t red8 = (rgb24 >> 16) & 0xFF;
	uint8_t green8 = (rgb24 >> 8) & 0xFF;
	uint8_t blue8 = rgb24 & 0xFF;
	uint8_t red = red8 >> 3;
	uint8_t green = green8 >> 2;
	uint8_t blue = blue8 >> 3;
	return { red, green, blue };
}
```

## Structure

Every Display Message is identified by the ItemType byte set to `0x04` and each one is distinguished by the *Function* byte.

| `F0 00 20 1A 16 ID#1 ID#2` | `0x04` | Function | `...message...` | `F7` | 
|:---:|:---:|:---:|:---:|:---:|
| Header | ItemType | | | | 

All the Display Messages flow in only one direction, from the Device to the SL.  
Here is a list of all the display messages with their data flow.

| System Message | Function byte | Direction |
|:---|:---:|:---:|
| Clear Screen | `0x01` | Device → SL |
| Draw Rectangle | `0x02` | Device → SL |
| Plot Bitmap | `0x03` | Device → SL |
| Write Text | `0x00` | Device → SL |

## Clear Screen Message
This message instructs the SL to perform a full monochromatic pain of the canvas.

| `F0 00 20 1A 16 ID#1 ID#2` | `0x04` | `0x01` | R | G | B | `F7` |
|:---:|:---:|:---:|:---|:---:|:---:|:---|
| Header | ItemType | Function | Color Red | Color Green | Color Blue | |

This is used to refresh the screen and prepare the canvas to draw a new user interface.  
Since this command results in a full screen update, it should be used with parsimony and only when  needed.  
If only a small portion of the screen must be updated the use of the Draw Rectangle Message is recommended.

## Draw Rectangle Message

The Draw Rectangle Message is structured as follow:

| `F0 00 20 1A 16 ID#1 ID#2` | `0x04` | `0x02` | XMSB | XLSB | YMSB | YLSB | WMSB | WLSB | HMSB | HLSB | R | G | B | `F7` |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
Header | Item Type | Function | X coord MSB | X coord LSB | Y coord MSB | Y coord LSB | Width MSB | Width LSB | Height MSB | Height LSB | Color Red | Color Green | Color Blue | |

Where:
- **XMSB**:	MSB of the x-coordinate of the top left pixel of the rectangle
- **XLSB**:	LSB of the x-coordinate of the top left pixel of the rectangle
- **YMSB**: MSB of the y-coordinate of the top left pixel of the rectangle
- **YLSB**: LSB of the y-coordinate of the top left pixel of the rectangle
- **WMSB**:	MSB of the width of the rectangle
- **WLSB**:	LSB of the width of the rectangle
- **HMSB**:	MSB of the height of the rectangle
- **HLSB**:	LSB of the height of the rectangle
- **R**: Red color byte
- **G**: Green color byte
- **B**: Blue color byte

This instruction draws a rectangle on the screen filled with the given color.  
The (X,Y) coordinate represents the top left corner of the rectangle, while the bottom right coordinate is given by the formula ( X + W - 1, Y + H - 1).

## Plot Bitmap Message
The Plot Bitmap Message allows the Device to access the internal SL bitmap library.  
The library is organized in Groups, each one containing a list of Icons.  
Each Group is indexed by a 7-bit value, in which the Device can access the proper Icon via another 7-bit index.  
For each Bitmap, the Device can choose the background and foreground color.

| `F0 00 20 1A 16 ID#1 ID#2` | `0x04` | `0x03` | XMSB | XLSB | YMSB | YLSB | GIDX | IIDX | FGR | FGG | FGB | BGR | BGG | BGB | `F7` |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
Header | Item Type | Function | X coord MSB | X coord LSB | Y coord MSB | Y coord LSB | Group Index | Icon Index | Foreground red | Foreground green | Foreground blue | Background red | Background green | Background blue | |

Where:
- **XMSB**: MSB of the x-coordinate of the top left pixel of the bitmap
- **XLSB**: LSB of the x-coordinate of the top left pixel of the bitmap
- **YMSB**: MSB of the y-coordinate of the top left pixel of the bitmap
- **YLSB**: LSB of the y-coordinate of the top left pixel of the bitmap
- **GIDX**: Bitmap Group index, see Appendix A
- **IIDX**: Bitmap Icon index, see Appendix A
- **FGR**: Foreground color Red value
- **FGG**: Foreground color Green value
- **FGB**: Foreground color Blue value
- **BGR**: Background color Red value
- **BGG**: Background color Green value
- **BGB**: Background color Blue value

Once requesting the selected Icon, the SL proceeds to compute the color gradient and apply it to the black and white image.  
It's important to know that the Icon will completely replace the content beneath it.   
This includes both the visible part of the Icon and its background, thus no alpha channel is considered.

A detailed list of Groups and Bitmaps, along with a preview and pixel dimension can be found in [Appendix A](appendix-a.md).

### Plot Device Icon Message

As explained in the [Device Icon Mechanism](system-messages.md#device-icon-mechanism) section, each Device can store one 32x32 pixel Logo in the SLMK2 RAM.  
To retrieve the Logo, a special kind of Plot Image message is required:

| `F0 00 20 1A 16 ID#1 ID#2` | `0x04` | `0x03` | XMLS | XMSB | YMSB | YLSB | `0x7F` | `F7` |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| Header | ItemType | Function | X coord MSB | X coord LSB | Y coord MSB | Y coord LSB | Logo request | |

As we can see, the message is in the same format, with a Group Id fixed to 0x7F and with no color information because of the custom colors stored in the image.  
For simplifying the programmer job, a Plot Image message with a Group Id of `0x7F` will ignore all the color bytes and the Icon Index byte, to allow the use of the same function as the Plot Bitmap one.

## Write Text Message
The write text message allows the Host Device to print a string on the LCD screen.

In addition to the (X,Y) coordinates, the routine accepts a Maximum Width argument.  
This argument refers to the pixel width of the string, and as every screen positional argument is composed of an MSB and LSB part.  
The SL guarantees that the given string will not exceed the Maximum Width given in pixels, truncating it and printing a three dot character if needed ('...').  
If the Maximum Width is set to zero, all the characters of the string will be printed, if it fit in the screen (no word wrap is implemented).

In the selected area (the area between (X, Y) and (X + Width, Y) the string can be justified to the left, right or center, according to the proper alignment byte in the message.

Moreover, the size of the text can be selected between *small* (21px), *medium* (27px), *big* (33px), via the size byte.

Also in this case there is a background and foreground color parameter, and like the precedent command the text will completely overwrite any existing content on the screen pixels within the area where the text is printed.  
This means that the text rendering will replace the underlying pixel data entirely, without preserving or blending with the previous content.

Finally the string is sent in 7-bit ASCII format, along with its string terminator.  
It must be noted that the SLMK2 fonts only contain ASCII characters from `0x20` to `0x80` included, and every other character sent will be replaced by a space.  
For this reason particual care must be taken in handling UNICODE, extended ASCII, or special character, that must be handled bu the Device before sending text to the SLMK2.
  
The message is structured as follows:

| `F0 00 20 1A 16 ID#1 ID#2` | `0x04` | `0x00` | XMSB | XLSB | YMSB | YLSB | WMSB | WLSB | ALIGN | SIZE | FGR | FGG | FGB | BGR | BGG | BGB | S(1) ... S(N) | `0x00` | `F7` |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
Header | Item Type | Function | X coord MSB | X coord LSB | Y coord MSB | Y coord LSB | Max width MSB | Max width LSB | Alignment | Size | Fg Red | Fg Green | Fg Blue | Bg red | Bg green | Bg blue | String to write | String terminator | |

Where:
- **XMSB**: MSB of the x-coordinate of the first character’s top left pixel
- **XLSB**: LSB of the x-coordinate of the first character’s top left pixel
- **YMSB**:	MSB of the y-coordinate of the first character’s top left pixel
- **YLSB**: LSB of the y-coordinate of the first character’s top left pixel
- **WMSB**: MSB of the width at which the string will be truncated (in pixels)
- **WLSB**: LSB of the width at which the string will be truncated (in pixels)
- **ALIG**:	Justification of the text in the given area, see the table below
- **SIZE**:	Size of the text, see the table below
- **FGR**: Foreground color Red value
- **FGG**: Foreground color Green value
- **FGB**: Foreground color Blue value
- **BGR**: Background color Red value
- **BGG**: Background color Green value
- **BGB**: Background color Blue value
- **S(1) ... S(N)**: The content of the string, in ASCII format
- **`0x00`**: The string terminator

The **ALIG** and **SIZE** bytes take values according to the following tables:

| ALIG | Resulting justification |
|:---:|:---:|
| `0x00` | Left |
| `0x01` | Center |
| `0x02` | Right |

| SIZE | Resulting size |
|:---:|:---:|
| `0x00` | Small (21px) |
| `0x01` | Medium (22px) |
| `0x02` | Big (33px) |

[Back to index](index.md)
<!-- [Back to index](../README.md) -->