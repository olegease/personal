        processor 6502
        ; includes
        ; -- system
        include "macro.h"
        include "vcs.h"
        ; -- local
        include "define.h"
        include "system_ntsc.h"
        ; constants
Lines_Overscan = System_Overscan;
Lines_Sync = System_Sync
Lines_Blank = System_Blank
Kernel_Size = System_Kernel
Kernel_Res = 1
Kernel_Blocks = ( Kernel_Size / Kernel_Res ) - 2
        ; RAM
        seg.u data
        org $80
        ; -- positions
OnePosX         .byte
OnePosY         .byte
TwoPosX         .byte
TwoPosY         .byte
        ; ROM
        seg code
        org $F000
Reset:
        CLEAN_START
        lda #Black_1
        sta COLUBK
Frame:
        ; kernel
        REPEAT Kernel_Res       ; NOTE: guard `blank` block
        sta WSYNC
        REPEND
        ldx #Kernel_Blocks
        jmp .kernelLoop
        ALIGN 256               ; NOTE: guarantees every block start at 5 cycles
.kernelLoop:                    ; TODO: write kernel logic
        REPEAT Kernel_Res
        sta WSYNC
        REPEND
        dex
        bne .kernelLoop
        REPEAT Kernel_Res       ; NOTE: guard `overscan` block
        sta WSYNC
        REPEND
        jsr _overscan_
        jsr _sync_
        jsr _blank_
        jmp Frame
        ; -- subroutines
_overscan_ SUBROUTINE           ; TODO: write physics logic
        lda #Turn_On
        sta VBLANK
        ldy #Lines_Overscan
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
_blank_ SUBROUTINE              ; TODO: write graphic logic
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
        ; END OF ROM
        org $FFFA
        .word Reset             ; NMI
        .word Reset             ; RESET
        .word Reset             ; IRQ / BRK
