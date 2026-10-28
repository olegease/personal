; HORizontal Player MoVment Example
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
Kernel_Res = 2
Kernel_Blocks = Kernel_Size / Kernel_Res - 2
        ; -- Player 0 (One)
One_Pos_X_Max = 153
One_Pox_X_Min = 2
One_Pos_X_Init = One_Pos_X_Max / 2
One_Pos_Y_Init = 8
One_Height = 2
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
        lda #$02
        sta COLUBK
        ; -- positions
        lda #One_Pos_X_Init
        sta OnePosX
        lda #One_Pos_Y_Init
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
        tay
        beq .kernelFinish
.kernelDraw:
        tay
        lda (OnePtrPixel),Y
.kernelFinish:
        sta WSYNC
        sta GRP0
        lda (OnePtrColor),Y
        sta COLUP0
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
.inputOneWest:
        lda #Joy0_Movebit_Lt
        bit SWCHA
        bne .inputOneEast
        lda OnePosX
        cmp #One_Pox_X_Min
        bcc .inputOneEast
        dec OnePosX
.inputOneEast:
        lda #Joy0_Movebit_Rt
        bit SWCHA
        bne .inputDone
        lda OnePosX
        cmp #One_Pos_X_Max
        bcs .inputDone
        inc OnePosX
.inputDone:
        lda OnePosX
        ldx #Zero
        jsr ax_hor_pos_2Y_
        ldy #Lines_Overscan - 3
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
        lda #Zero
        sta GRP0
        ldy #Lines_Blank
        jsr y_wsync_loop_
        lda #Turn_Off
        sta VBLANK
        rts
y_wsync_loop_ SUBROUTINE        ; in: Y number to repeat, out: none
        sta WSYNC
        dey
        bne y_wsync_loop_
        rts
ax_hor_pos_2Y_ SUBROUTINE       ; in: A x-coord, X object index, out: none
        sta WSYNC
        nop
        sec
.div15Loop:
        sbc #15
        bcs .div15Loop
        tay
        lda HorAdjustTable,Y
        sta HMP0,X
        sta RESP0,X
        sta WSYNC
        sta HMOVE
        rts
        ; data
        ALIGN 256
HorAdjustValues:.byte $70,$60,$50,$40,$30,$20,$10,$00,$F0,$E0,$D0,$C0,$B0,$A0,$90
HorAdjustTable = HorAdjustValues - $F1
        ; sprites
OneColor:       .byte   $1E,$8E
OnePixel:       .byte   $FF,$FF
        ; END OF ROM
        org $FFFA
        .word Reset             ; NMI
        .word Reset             ; RESET
        .word Reset             ; IRQ / BRK
