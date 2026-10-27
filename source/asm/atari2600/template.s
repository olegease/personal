        processor 6502
        ; includes
        ; -- system
        include "macro.h"
        include "vcs.h"
        ; -- local
        include "define.h"
        ; constants
Lines_Overscan = NTSC_Overscan  ;
Lines_Vsync = NTSC_Sync         ;
Lines_Blank = NTSC_Blank        ;
Lines_Size = 1                  ;
        ; -- kernel draw size, NOTE: so each draw cycle including first would have same cycles
Kernel_Size = NTSC_Kernel      ;
Kernel_Res = 1
Kernel_Loop = Kernel_Size - 2 * Kernel_Res
        ; RAM
        seg.u data              ;
        org $80                 ;
        ; -- positions
OnePosX         .byte           ;
OnePosY         .byte           ;
TwoPosX         .byte           ;
TwoPosY         .byte           ;
        ; ROM
        seg code                ;
        org $F000               ;
Reset:                          ;
        CLEAN_START             ;
        lda #Black_1            ;
        sta COLUBK              ;
Frame:                          ;
        ; kernel
        sta WSYNC               ;
        lda #Turn_Off           ;
        sta VBLANK              ;
        REPEAT Kernel_Res - 1   ;
        sta WSYNC               ;
        REPEND                  ;
        ldx #Kernel_Loop        ;
.kernelLoop:                    ; TODO: write kernel logic
        REPEAT Kernel_Res       ;
        sta WSYNC               ;
        dex                     ;
        REPEND                  ;
        bne .kernelLoop         ;
        REPEAT Kernel_Res       ;
        sta WSYNC               ;
        REPEND                  ;
        lda #Turn_On            ;
        sta VBLANK              ;
        jsr _ntsc_overscan_     ; 30 scan lines for physics
        jsr _ntsc_vsync_        ; 3 scan lines for synchronize vertical sync
        jsr _ntsc_blank_        ; 37 scan lines for graphics
        jmp Frame               ;
        ; -- subroutines
_ntsc_overscan_ SUBROUTINE      ; TODO: write physics logic
        ldy #Lines_Overscan     ;
        jsr y_wsync_loop_       ;
        rts                     ;
_ntsc_vsync_ SUBROUTINE         ; NOTE: do not touch
        lda #Turn_On            ;
        sta VSYNC               ;
        ldy #Lines_Vsync;
        jsr y_wsync_loop_       ;
        lda #Turn_Off           ;
        sta VSYNC               ;
        rts                     ;
_ntsc_blank_ SUBROUTINE         ; TODO: write graphic logic
        ldy #Lines_Blank        ;
        jsr y_wsync_loop_       ;
        rts                     ;
y_wsync_loop_ SUBROUTINE        ; in: Y number to repeat, out: none
        sta WSYNC               ;
        dey                     ;
        bne y_wsync_loop_       ;
        rts                     ;
        ; END OF ROM
        org $FFFA               ;
        .word Reset             ;
        .word Reset             ;
        .word Reset             ;
