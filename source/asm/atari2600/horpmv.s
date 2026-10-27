; HORizontal Player MoVment Example
        processor 6502
        ; includes
        include "macro.h"
        include "vcs.h"
        ; constants
ZERO = 0
NTSC_VISIBLE = 192              ;
NTSC_OVERSCAN = 30              ;
NTSC_VSYNC = 3                  ;
NTSC_BLANK = 36                 ; NOTE: borrowed one line for visible lines
ONE_INIT_POS_X = 140            ;
ONE_INIT_POS_Y = 8              ;
ONE_HEIGHT = 2                  ;
        ; RAM
        seg.u data              ;
        org $80                 ;
        ; -- positions
OnePosX         .byte           ;
OnePosY         .byte           ;
OnePtrColor     .word
OnePtrPixel     .word
        ; ROM
        seg code                ;
        org $F000               ;
Reset:                          ;
        CLEAN_START             ;
        ; -- colors
        lda #$02                ;
        sta COLUBK              ;
        ; -- positions
        lda #ONE_INIT_POS_X
        sta OnePosX
        lda #ONE_INIT_POS_Y
        sta OnePosY
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
        ldx #NTSC_VISIBLE / 2   ; 2-line kernel
.kernelLoop:
        txa                     ;
        sec                     ;
        sbc OnePosY             ;
        cmp #ONE_HEIGHT         ;
        bcc .drawOne            ;
        lda #ZERO               ;
        tay
        beq .kernelFinish       ;
.drawOne:                       ;
        tay                     ;
        lda (OnePtrPixel),Y     ;
.kernelFinish:                  ;
        sta WSYNC               ;
        sta GRP0                ;
        lda (OnePtrColor),Y     ;
        sta COLUP0              ;
        sta WSYNC               ;
        dex                     ;
        bne .kernelLoop         ;
        ; kernel logic before turning on `VBLANK`
        lda #%00000010          ;
        sta VBLANK              ;
        jsr _ntsc_overscan_     ; 30 scan lines for physics
        jsr _ntsc_vsync_        ; 3 scan lines for synchronize vertical sync
        jsr _ntsc_blank_        ; 37 scan lines for graphics
        jmp Frame               ;
        ; -- subroutines
_ntsc_overscan_ SUBROUTINE      ;
        ; input
.inputOneWest:                  ;
        lda #%01000000          ;
        bit SWCHA               ;
        bne .inputOneEast       ;
        dec OnePosX             ;
.inputOneEast:                  ;
        lda #%10000000          ;
        bit SWCHA               ;
        bne .inputDone          ;
        inc OnePosX             ;
.inputDone:                     ;
        lda OnePosX             ;
        ldx #ZERO               ;
        jsr ax_hor_pos_2Y_      ;
        ldy #NTSC_OVERSCAN - 2  ;
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
        lda #ZERO               ;
        sta GRP0                ;
        ldy #NTSC_BLANK         ;
        jsr y_wsync_loop_       ;
        rts                     ;
y_wsync_loop_ SUBROUTINE        ; in: Y number to repeat, out: none
        sta WSYNC               ;
        dey                     ;
        bne y_wsync_loop_       ;
        rts                     ;
ax_hor_pos_2Y_ SUBROUTINE       ; in: A x-coord, X object index, out: none
        sta WSYNC               ;
        nop                     ;
        sec                     ;
.div15Loop:                     ;
        sbc #15                 ;
        bcs .div15Loop          ;
        tay                     ;
        lda HorAdjustTable,Y    ;
        sta HMP0,X              ;
        sta RESP0,X             ;
        sta WSYNC               ;
        sta HMOVE               ;
        rts                     ;
        ; data
        ALIGN 256
HorAdjustValues:
        .byte $70, $60, $50, $40, $30, $20, $10, $00
        .byte $F0, $E0, $D0, $C0, $B0, $A0, $90
HorAdjustTable = HorAdjustValues - $F1
        ; sprites
OneColor:       .byte   #$1E,#$8E
OnePixel:       .byte   #$FF,#$FF
        ; END OF ROM
        org $FFFA               ;
        .word Reset             ;
        .word Reset             ;
        .word Reset             ;
