; VERtical Player MoVement example
        processor 6502
        ; includes
        ; -- system
        include "macro.h"
        include "vcs.h"
        ; -- local
        include "define.h"
        include "system_ntsc.h"
        ; constants
        ; -- lines
Lines_Overscan = System_Overscan
Lines_Sync = System_Sync
Lines_Blank = System_Blank
        ; -- kernel
Kernel_Size = System_Kernel
Kernel_Res = 3
Kernel_Blocks = Kernel_Size / Kernel_Res - 2
        ; -- Player 0 (One)
One_Height = 8
One_Pos_Y_Max = Kernel_Blocks - One_Height
One_Pos_Y_Min = Kernel_Res - 1
One_Pos_Init_X = 8 ; TODO: fix bottom display glitch at west edge
One_Pos_Init_Y = One_Pos_Y_Max / 2
        ; RAM
        seg.u data
        org $80
        ; -- positions
OnePosX         .byte
OnePosY         .byte
OnePtrColor     .word
OnePtrPixel     .word
        ; ROM
        seg code
        org $F000
Reset:
        CLEAN_START
        ; -- colors
        lda #Black_1
        sta COLUBK
        ; -- positions
        lda #One_Pos_Init_X
        sta OnePosX
        lda #One_Pos_Init_Y
        sta OnePosY
        ; -- pointers
        SET_POINTER OnePtrColor,OneColor
        SET_POINTER OnePtrPixel,OnePixel
Frame:
        ; kernel
        REPEAT Kernel_Res       ; NOTE: guard `blank` block
        sta WSYNC
        REPEND
        ldx #Kernel_Blocks
        jmp Kernel
        ALIGN 256               ; NOTE: guarantees every block start at 5 cycles
Kernel:
        txa
        sec
        sbc OnePosY
        cmp #One_Height
        bcc .kernelDraw
        lda #Zero
        sta GRP0
        beq .kernelFinish
.kernelDraw:
        tay
        lda (OnePtrPixel),Y
        sta GRP0
        lda (OnePtrColor),Y
        sta COLUP0
.kernelFinish:
        sta WSYNC
        sta WSYNC
        sta WSYNC
        dex
        bne Kernel
        REPEAT Kernel_Res       ; NOTE: guard `overscan` block
        sta WSYNC
        REPEND
        jsr _overscan_
        jsr _sync_
        jsr _blank_
        jmp Frame
        ; -- subroutines
_overscan_ SUBROUTINE
        lda #Turn_On
        sta VBLANK
        sta WSYNC
        ; input
.inputOneNorth:
        lda #Joy0_Movebit_Up
        bit SWCHA
        bne .inputOneSouth
        lda OnePosY
        cmp #One_Pos_Y_Max
        bcs .inputOneSouth
        inc OnePosY
.inputOneSouth:
        lda #Joy0_Movebit_Dn
        bit SWCHA
        bne .inputDone
        lda OnePosY
        cmp #One_Pos_Y_Min + 1
        bcc .inputDone
        dec OnePosY
.inputDone:
        sta WSYNC
        ; remaining lines
        ldy #Lines_Overscan - 2
        jsr y_wsync_loop_
        rts
_sync_ SUBROUTINE               ; NOTE: do not touch
        lda #Turn_On
        sta VSYNC
        ldy #Lines_Sync
        jsr y_wsync_loop_
        lda #Turn_Off
        sta VSYNC
        rts
_blank_ SUBROUTINE
        sta WSYNC
        ; clearing graphics
        lda #Zero
        sta GRP0
        ; dirty horizontal positioning
        lda OnePosX
        sta WSYNC
        sec
.dirtyOnePosX:
        sbc #15
        bcs .dirtyOnePosX
        sta RESP0
        sta WSYNC
        sta HMOVE
        ldy #Lines_Blank - 3
        jsr y_wsync_loop_
        lda #Turn_Off
        sta VBLANK
        rts
y_wsync_loop_ SUBROUTINE        ; in: Y number to repeat
        sta WSYNC
        dey
        bne y_wsync_loop_
        rts
        ; sprites
OneColor:       .byte   #$40,#$1E,#$1E,#$1E,#$8E,#$8E,#$8E,#$40
OnePixel:       .byte   #$18,#$18,#$18,#$18,#$18,#$18,#$18,#$18
        ; END OF ROM
        org $FFFA
        .word Reset             ; NMI
        .word Reset             ; RESET
        .word Reset             ; IRQ / BRK
