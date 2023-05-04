# A Simple Video Display Processor for the Micro-Professor MPF-1

<img src="https://github.com/dadecoza/arduino-mpf1-vdp/blob/main/Images/MPF1_VDP_Demo.jpg?raw=true" width="640">

## Things you will need
### Hardware
* Arduino UNO
* 1x GAL20V8 PLD IC
* 1x 470 Ohm resistor
* 1x 1K Ohm resistor
* Programmer for the GAL20V8. The popular TL866 works well
### Software
* The Arduino IDE
* WinCupl (only if you want to change the GAL20V8 pin assignments)
* MiniPro Programmer software (in the case of the TL866)

## The circuit
<img src="https://github.com/dadecoza/arduino-mpf1-vdp/blob/main/Images/MPF1_VDP_Circuit.png?raw=true" width="640">

## The MPF-1 P1 expansion port pinouts
* Source: http://www.1000bit.it/support/manuali/multitech/MPF-I-User's-manual.pdf
<img src="https://github.com/dadecoza/arduino-mpf1-vdp/blob/main/Images/MPF1_Jumper1.PNG?raw=true">

## Programming the GAL20V8
* I have included the <a href="https://github.com/dadecoza/arduino-mpf1-vdp/tree/main/Files">.JED</a> file that you can write to the PLD for use with the circuit above.
* You can use WinCupl if you want to change the pinouts or use a different PLD.
<img src="https://github.com/dadecoza/arduino-mpf1-vdp/blob/main/Images/MPF1_VDP_WinCupl.png?raw=true" width="640">

## Loading the Arduino sketch
* I have included my <a href="https://github.com/dadecoza/arduino-mpf1-vdp/tree/main/Arduino">Arduino directory</a>, you will have to copy the contents to your own sketchbook directory.
* I have also included a patched copy of the TVOut library with some added features (ie. cursor position, invert text, mirror text ...).
## Usage
You have probably noticed that only 4 bits of the 8-bit bus on the Z80 are connected to the PLD and from the PLD to the Arduino.

This means we will have to send the byte nibble for nibble at a time.

We also need to warn the Arduino VDP that we are about to send a byte and we do this by first sending 0xF and only then sending the two nibbles (first high, then low).

The following subroutine will do the needed work to write a byte to the Arduino VDP ...
```asm
VDP     PUSH    BC              ; start of VDP subroutine ... copy BC to the stack for save keeping
        LD      (VDPB), A       ; store the byte to display in memory
        LD      C, 0xC0         ; C contains the IO address for the VDP
        LD      A, 0xFF         ; load 0xFF into the accumulator
        OUT     (C), A          ; Write the 0xFF to the VDP indicating that the high nibble of the byte will follow 
        LD      B, 50           ; load 50 into the B register for a delay loop
VDPD1   DJNZ    VDPD1           ; do the delay loop so that the VDP microcontroller has some time to do its thing
        LD      A, (VDPB)       ; load the byte we want to load back into the accumulator 
        RRA                     ; rotate right ... wewantt to get to the high nibble
        RRA                     ; rotate some more
        RRA                     ; and again
        RRA                     ; yay we made it
        OUT     (C), A          ; write the high nibble to the VDP
        LD      B, 50           ; again with the delay loop ... that microcontroller is running at 16mhz!
VDPD2   DJNZ    VDPD2           ; loopy loop
        LD      A, (VDPB)       ; load the original byte we want to display back into the A register
        LD      B, $0F          ; bitmask to get rid of the high nibble leaving only the lower
        AND     B               ; apply the bitmask to the accumulator
        OUT     (C), A          ; write the low nibble to the VDP
        LD      B, 200          ; final delay loop ... if we weren't so lazy wecould havee implementedd an ACK signal
VDPD3   DJNZ    VDPD3           ; last loopy loop
        POP     BC              ; recover BC
        RET                     ; return from subroutine
```
See the complete demo listing <a href="https://github.com/dadecoza/arduino-mpf1-vdp/blob/main/Examples/MPF1_VDP_Demo.asm">here</a> ...

## Escape Code Examples
### Cursor Position
* Move the cursor to row 5 column 10
```asm
0x1B, 0x1, 10, 5
```
### Change the font
```asm
0x1B, 0x2, 0x1  ; font4x6
0x1B, 0x2, 0x2  ; font6x8
0x1B, 0x2, 0x3  ; font8x8
0x1B, 0x2, 0x4  ; font8x8ext
```
### Draw a line
* Draw a line from 5,5 (x,y) and ending at 20, 20
```asm
0x1B, 0x3, 5, 5, 20, 20
```
### Draw a rectangle
* Draw a rectangle at position 5,5 (x,y) with a width of 20 pixels and height of 10 pixels.
```asm
0x1B, 0x4, 5, 5, 20, 10
```
### Draw a filled rectangle
* Draw a filled rectangle at position 5,5 (x,y) with a width of 20 pixels and height of 10 pixels.
```asm
0x1B, 0x5, 5, 5, 20, 10
```
### Draw a circle
* Draw a circle at position 5,5 (x,y) with a radius of 10.
```asm
0x1B, 0x6, 5, 5, 10
```
### Draw a filled circle
* Draw a filled circle at position 5,5 (x,y) with a radius of 10.
```asm
0x1B, 0x7, 5, 5, 10
```
### Invert the text colour (black on white)
```asm
0x1B, 0x8, 0x2
```
### Invert the text colour (default)
```asm
0x1B, 0x8, 0x1
```
### Enable the cursor
```asm
0x1B, 0x9, 0x2
```
### Disable the cursor (default)
```asm
0x1B, 0x9, 0x1
```
### Enable mirroring of text characters (handy for custom fonts containing sprites)
```asm
0x1B, 0x10, 0x2
```
### Disable mirroring of text characters (default)
```asm
0x1B, 0x10, 0x1
```

# Credits / Resources / Tools
* Obviously the heavy lifting is done by the awesome TVOut library ... https://github.com/Avamander/arduino-tvout
* The very useful Micro-Professor MPF-1 users manual ... http://www.1000bit.it/support/manuali/multitech/MPF-I-User's-manual.pdf
* The GAL20V8 datasheet ... https://www.latticesemi.com/-/media/LatticeSemi/Documents/DataSheets/GAL/GAL20V8DataSheet.ashx?la=en
* The Atmega168/328 pinouts ... https://docs.arduino.cc/static/47180601da55c458736e09d26b8bfab5/d0d8c/Atmega168PinMap2.png
* Excelent video on how to use WinCupl and defining logic for GAL/PLD chips ... https://www.youtube.com/watch?v=fo_WlA-kVRc
* z80-asm assembler ... http://wwwhomes.uni-bielefeld.de/achim/z80-asm.html
* Arduino direct port manipulation ... https://docs.arduino.cc/hacking/software/PortManipulation

