        ORG     0x1800
INIT    LD      IY, DATA        ; IY points to the video data start address
DISP    LD      A, (IY)         ; load byte of data from memory
        OR      A               ; null termination?
        JR      Z, DONE         ; if so, then we are done
        CALL    VDP             ; call the VDP subroutine to write the byte to the VDP
        INC     IY              ; increment IY to move to next byte
        JR      DISP            ; loop back to display
DONE    HALT                    ; nothing more to do!
VDP     PUSH    BC              ; start of VDP subroutine ... copy BC to the stack for save keeping
        LD      (VDPB), A       ; store the byte to display in memory
        LD      C, 0xC0         ; C contains the IO address for the VDP
        LD      A, 0xFF         ; load 0xFF into the accumulator
        OUT     (C), A          ; Write the 0xFF to the VDP indicating that the high nibble of the byte will follow 
        LD      B, 50           ; load 50 into the B register for a delay loop
VDPD1   DJNZ    VDPD1           ; do the delay loop so that the VDP microcontroller have some time to do it's thing
        LD      A, (VDPB)       ; load the byte we want to load back into the accumulator 
	    RRA                     ; rotate right ... we wan't to get to the high nibble
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
        LD      B, 200          ; final delay loop ... if we weren't so lazy we could have implented an ACK signal
VDPD3   DJNZ    VDPD3           ; last loopy loop
        POP     BC              ; recover BC
        RET                     ; return from subroutine
VDPB    DEFB    0x0             ; a place to store the byte we are going to write to the VDP
DATA    DEFB    0x1B, 0x2, 0x1                   ; The data and commands we will be sending... starting by setting the font to Font4x6 
        DEFB    0xC, ' ','M','i','n','i','m','a','l','i','s','t'
        DEFB    ' ','C','o','m','p','u','t','i','n','g', ' '
        DEFB    'o', 'n',' ','a',' ','.','.','.',0xA
        DEFB    0x1B, 0x2, 0x3                   ; larger font 8x8
        DEFB    'M','i','c','r','o','-','P','r','o','f','e','s'
        DEFB    's','o','r',' '                  ; Some text
        DEFB    0x1B, 0x3, 1, 95, 127, 95        ; Grass (Line)
        DEFB    0x1B, 0x4, 90, 70, 20, 24        ; House (Rectangle)
        DEFB    0x1B, 0x3, 90, 70, 100, 55       ; Roof left (Line)
        DEFB    0x1B, 0x3, 101, 54, 110, 70      ; Roof right (Line)
        DEFB    0x1B, 0x4, 98, 85, 4, 8          ; Door (Rectangle)
        DEFB    0x1B, 0x4, 93, 73, 4, 4          ; Window (Left)
        DEFB    0x1B, 0x4, 103, 73, 4, 4         ; Window (Right)
        DEFB    0x1B, 0x4, 101, 89, 1, 1         ; Door knob (Rectangle)
        DEFB    0x1B, 0x6, 20, 30, 6             ; Sun (Circle)
        DEFB    0x1B, 0x3, 27, 37, 32, 42        ; Sunray 1 (Line)
        DEFB    0x1B, 0x3, 29, 35, 34, 40        ; Sunray 2 (Line)
        DEFB    0x1B, 0x3, 25, 39, 30, 44        ; Sunray 3 (Line)
        DEFB    0x0                              ; Null terminator
        END