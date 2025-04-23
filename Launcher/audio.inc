section .data
    ; === Note Frequency Constants ===
    SILENCE equ 0
    C4 equ 261
    Cs4 equ 277    ; C#4 / Db4
    D4 equ 293
    Ds4 equ 311
    E4 equ 329
    F4 equ 349
    Fs4 equ 370
    G4 equ 392
    Gs4 equ 415
    A4 equ 440
    As4 equ 466
    B4 equ 493
    C5 equ 523
    Cs5 equ 554
    D5 equ 587
    Ds5 equ 622
    E5 equ 659
    F5 equ 698
    Fs5 equ 740
    G5 equ 784
    Gs5 equ 831
    A5 equ 880
    As5 equ 932
    B5 equ 987
    C6 equ 1046

    ; === Notes and durations ===
    currentNotes:
        dw A4
        dw 0

    currentDurations:
        dw 50
        dw 0

section .text
    playSound:
        ; Set pointers to notes and durations
        mov si, currentNotes
        mov di, currentDurations

    .next_note:
        mov ax, [si]       ; Get frequency
        mov cx, [di]       ; Get duration

        cmp ax, 0
        jne .play_note

        cmp cx, 0
        je .done           ; Both zero = END
        call delay         ; Silence with duration
        jmp .advance

    .play_note:
        call beep

    .advance:
        add si, 2
        add di, 2
        jmp .next_note

    .done:
        ret

    ; ------------------------
    ; beep: Plays a tone
    ; IN: AX = frequency in Hz
    ;     CX = duration loop
    ; ------------------------
    beep:
        cmp ax, 0
        je .pause        ; 0 Hz = pause

        push dx
        push bx

        mov bx, ax         ; Save frequency

        ; Load 1193182 into DX:AX (0x0012C2EE)
        mov ax, 0xC2EE
        mov dx, 0x0012
        div bx             ; DX:AX / BX → AX = divisor

        mov bx, ax         ; Store divisor

        ; Program PIT channel 2
        mov al, 0b10110110
        out 43h, al

        mov al, bl
        out 42h, al
        mov al, bh
        out 42h, al

        ; Turn on speaker
        in al, 61h
        or al, 00000011b
        out 61h, al

        call delay

        ; Turn off speaker
        in al, 61h
        and al, 11111100b
        out 61h, al

        pop bx
        pop dx
        ret

    .pause:
        call delay
        ret

    ; ------------------------
    ; delay: simple busy-loop
    ; IN: CX = loop count
    ; ------------------------
    delay:
        push cx
        push dx
    .outer:
        mov dx, 0FFFFh
    .inner:
        dec dx
        jnz .inner
        loop .outer
        pop dx
        pop cx
        ret
