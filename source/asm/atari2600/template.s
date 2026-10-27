        processor 6502
        ; includes
        include "macro.h"
        include "vcs.h"
        ; constants
NTSC_VISIBLE = 192
NTSC_OVERSCAN = 30
NTSC_VSYNC = 3
NTSC_BLANK = 37
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
        lda #$02                ;
        sta COLUBK              ;
Frame:                          ;
        jsr _ntsc_visible_      ; 192 scan lines for drawing
        jsr _ntsc_overscan_     ; 30 scan lines for input and physics
        jsr _ntsc_vsync_        ; 3 scan lines for synchronize vertical sync
        jsr _ntsc_blank_        ; 37 scan lines for graphics
        jmp Frame               ;
        ; -- subroutines
_ntsc_visible_ SUBROUTINE       ;
        ldy #NTSC_VISIBLE       ;
        jsr y_wsync_loop_       ;
        lda #%00000010          ;
        sta VBLANK              ;
        rts                     ;
_ntsc_overscan_ SUBROUTINE      ;
        ldy #NTSC_OVERSCAN      ;
        jsr y_wsync_loop_       ;
        rts                     ;
_ntsc_vsync_ SUBROUTINE         ;
        lda #%00000010          ;
        sta VSYNC               ;
        ldy #NTSC_VSYNC         ;
        jsr y_wsync_loop_       ;
        lda #%00000000          ;
        sta VSYNC               ;
        rts                     ;
_ntsc_blank_ SUBROUTINE         ;
        ldy #NTSC_BLANK         ;
        jsr y_wsync_loop_       ;
        lda #%00000000          ;
        sta VBLANK              ;
        rts                     ;
y_wsync_loop_ SUBROUTINE        ; in: Y number to repeat
        sta WSYNC               ;
        dey                     ;
        bne y_wsync_loop_       ;
        rts                     ;
        ; END OF ROM
        org $FFFC               ;
        .word Reset             ;
        .word Reset             ;
