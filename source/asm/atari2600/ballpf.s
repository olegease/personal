; BALL Play Field example
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
Kernel_Res = 1
Kernel_Blocks = ( Kernel_Size / Kernel_Res ) - 2
        ; -- ball
Ball_Pox_X_Init = 80
Ball_Pox_Y_Init = Kernel_Blocks / 2
        ; RAM
        seg.u data
        org Data_RAM
        ; -- positions
BallPosX        .byte
BallPosY        .byte
        ; ROM
        seg code
        org Code_ROM_Start
Reset:
        CLEAN_START
        lda #Red_4
        sta COLUBK
        ldx #Ball_Pox_X_Init
        stx BallPosX
        ldy #Ball_Pox_Y_Init
        sty BallPosY
Frame:
        ; kernel
        REPEAT Kernel_Res       ; NOTE: guard `blank` block
        sta WSYNC
        REPEND
        ldx #Kernel_Blocks
        jmp Kernel
        ALIGN Page_Boundary     ; NOTE: guarantees every block start at 5 cycles
Kernel:                         ; TODO: write kernel logic
        REPEAT Kernel_Res
        sta WSYNC
        REPEND
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
        org Code_ROM_Final
        .word Reset             ; NMI
        .word Reset             ; RESET
        .word Reset             ; IRQ / BRK
