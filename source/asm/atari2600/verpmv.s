; VERtical Player MoVement example
        processor 6502
        ; includes
        include "macro.h"
        include "vcs.h"
        ; constants
ZERO = 0
ONE_HEIGHT = 8
ONE_POS_Y_MAX = 65 - ONE_HEIGHT ;
ONE_POS_Y_MIN = 1               ;
NTSC_VISIBLE = 192              ;
NTSC_OVERSCAN = 30              ;
NTSC_VSYNC = 3                  ;
NTSC_BLANK = 36                 ; NOTE: borrowed one line for visible lines
        ; RAM
        seg.u data              ;
        org $80                 ;
        ; -- positions
OnePosX         .byte           ;
OnePosY         .byte           ;
OnePtrColor     .word           ;
OnePtrPixel     .word           ;
        ; ROM
        seg code                ;
        org $F000               ;
Reset:                          ;
        CLEAN_START             ;
        ; -- colors
        lda #$02                ;
        sta COLUBK              ;
        ; -- positions
        lda #1                  ;
        sta OnePosX             ;
        lda #32                 ;
        sta OnePosY             ;
        ; -- pointers
        lda #<OneColor          ;
        sta OnePtrColor         ;
        lda #>OneColor          ;
        sta OnePtrColor+1       ;
        lda #<OnePixel          ;
        sta OnePtrPixel         ;
        lda #>OnePixel          ;
        sta OnePtrPixel+1       ;
Frame:                          ;
        sta WSYNC               ;
        lda #%00000000          ;
        sta VBLANK              ;
        ldx #NTSC_VISIBLE / 3   ; 3-line kernel
.kernelLoop:                    ;
        txa                     ;
        sec                     ;
        sbc OnePosY             ;
        cmp #ONE_HEIGHT         ;
        bcc .drawOne            ;
        lda #ZERO               ;
        sta GRP0                ;
        beq .kernelFinish       ;
.drawOne:                       ;
        tay                     ;
        lda (OnePtrPixel),Y     ;
        sta GRP0                ;
        lda (OnePtrColor),Y     ;
        sta COLUP0              ;
.kernelFinish:                  ;
        sta WSYNC               ;
        sta WSYNC               ;
        sta WSYNC               ;
        dex                     ;
        bne .kernelLoop         ;
        lda #%00000010          ;
        sta VBLANK              ;
        jsr _ntsc_overscan_     ; 30 scan lines for input and physics
        jsr _ntsc_vsync_        ; 3 scan lines for synchronize vertical sync
        jsr _ntsc_blank_        ; 37 scan lines for graphics
        jmp Frame               ;
        ; -- subroutines
_ntsc_overscan_ SUBROUTINE      ;
        ; input
.inputOneNorth:                 ;
        lda #%00010000          ;
        bit SWCHA               ;
        bne .inputOneSouth      ;
        lda OnePosY             ;
        cmp #ONE_POS_Y_MAX      ;
        bcs .inputOneSouth      ;
        inc OnePosY             ;
.inputOneSouth:                 ;
        lda #%00100000          ;
        bit SWCHA               ;
        bne .inputDone          ;
        lda OnePosY             ;
        cmp #ONE_POS_Y_MIN + 1  ;
        bcc .inputDone          ;
        dec OnePosY             ;
.inputDone:                     ;
        sta WSYNC               ;
        ; remaining lines       ;
        ldy #NTSC_OVERSCAN - 1  ;
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
        ; clearing graphics
        lda #ZERO               ;
        sta GRP0                ;
        ; dirty horizontal positioning
        lda OnePosX             ;
        sta WSYNC               ;
        sec                     ;
.dirtyOnePosX:                  ;
        sbc #15                 ;
        bcs .dirtyOnePosX       ;
        sta RESP0               ;
        sta WSYNC               ;
        ldy #NTSC_BLANK - 2     ;
        jsr y_wsync_loop_       ;
        rts                     ;
y_wsync_loop_ SUBROUTINE        ; in: Y number to repeat
        sta WSYNC               ;
        dey                     ;
        bne y_wsync_loop_       ;
        rts                     ;
        ; sprites
OneColor:       .byte   #$40,#$40,#$08,#$08,#$08,#$08,#$40,#$40
OnePixel:       .byte   #$18,#$18,#$18,#$18,#$18,#$18,#$18,#$18
        ; END OF ROM
        org $FFFA               ;
        .word Reset             ;
        .word Reset             ;
        .word Reset             ;
