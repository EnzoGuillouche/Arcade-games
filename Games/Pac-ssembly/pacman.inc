%macro colTest 6
    %6:
        mov ax, %1
        sub ax, %4
        cmp word %2, -2
        jne .testOtherSide
        add ax, 2
        jmp .endCol
        .testOtherSide:
        cmp word %2, 2
        jne .sideCol
        add ax, 9
        jmp .endCol
        .sideCol:
        cmp byte %3, 1
        je .firstSideCol
        add ax, 5
        .firstSideCol:
        add ax, 3
        .endCol:
        mov bx, %5
        xor dx, dx
        div bx
%endmacro

%macro colVerifier 2
    cmp al, 32
    je tunnelTeleport
    cmp al, 33
    je tunnelTeleport
    cmp al, 35
    je %1
    cmp al, 36
    je %1
    cmp al, 37
    je %1
    jmp %2
%endmacro

%macro checkCollisions 8
    %1:
        colTest [pacman_y_pos], %3, %4, MAZE_Y_POS, WALL_HEIGHT, %6
        mov bx, COLUMN_NUMBER
        xor dx, dx
        mul bx
        push ax

        colTest [pacman_x_pos], %2, %4, MAZE_X_POS, WALL_WIDTH, %5
        mov bx, ax
        pop ax

        add bx, ax
        mov al, [maze+bx]

        mov bx, %7
        cmp byte %4, 1
        jne .secondColTest
        mov byte %4, 2
        colVerifier movementLoop, %8
        .secondColTest:
        mov byte %4, 1
        colVerifier pacmanUpdateDirection, %8
%endmacro

%macro pacmanAnimationSide 2
    %1:
        cmp byte [pacman_animation_frame], 2
        je .pacmanFrame1
        cmp byte [pacman_animation_frame], 4
        je .pacmanFrame2
        cmp byte [pacman_animation_frame], 6
        je .pacmanFrame3
        cmp byte [pacman_animation_frame], 8
        je .pacmanFrame4
        ret
        .pacmanFrame1:
            inc byte [item+0]
            ret
        .pacmanFrame2:
            dec byte [item+0]
            ret
        .pacmanFrame3:
            mov byte [item+0], 0
            ret
        .pacmanFrame4:
            mov byte [item+0], %2
            jmp .endAnimation
        .endAnimation:
        mov byte [pacman_animation_frame], 0
        ret
%endmacro

%macro updateAnimation 1
    mov byte [item+0], %1
    mov byte [pacman_animation_frame], 0
    jmp doMovement
%endmacro

section .text
    pacmanMovement:
        mov byte [col_test_nb], 1
        mov byte [col_test_nb_buffer], 1
        movementLoop:
        cmp word [pacman_x_speed_buffer], 0
        jne checkBuffer
        cmp word [pacman_y_speed_buffer], 0
        jne checkBuffer
        
        checkActual:
        checkCollisions actualMovement, [pacman_x_speed], [pacman_y_speed], [col_test_nb], colX, colY, 0, skipMovement
        
        checkBuffer:
        checkCollisions bufferMovement, [pacman_x_speed_buffer], [pacman_y_speed_buffer], [col_test_nb_buffer], colX_buffer, colY_buffer, 1, checkActual

    doMovement:
        inc byte [pacman_animation_frame]
        mov ax, [pacman_x_speed]
        add word [pacman_x_pos], ax
        mov ax, [pacman_y_speed]
        add word [pacman_y_pos], ax
        ret
    skipMovement:
        mov byte [col_test_nb], 1
        mov byte [pacman_animation_frame], 8
        ret
    tunnelTeleport:
        inc byte [pacman_animation_frame]
        cmp word [pacman_x_speed], -2
        je .tpToRight
        sub word [pacman_x_pos], WALL_WIDTH * 25
        ret
    .tpToRight:
        add word [pacman_x_pos], WALL_WIDTH * 25
        ret

    pacmanUpdateDirection:
        cmp bx, 0
        je .skipUpdate

        ; update speeds
        mov ax, [pacman_x_speed_buffer]
        mov bx, [pacman_y_speed_buffer]
        mov word [pacman_x_speed], ax
        mov word [pacman_y_speed], bx

        ; reset buffers
        mov word [pacman_x_speed_buffer], 0
        mov word [pacman_y_speed_buffer], 0

        ; update anims
        cmp word [pacman_x_speed], 0
        jl .left
        jg .right
        cmp word [pacman_y_speed], 0
        jg .down
        jmp .skipUpdate
        .up:
            updateAnimation 1
        .right:
            updateAnimation 3
        .down:
            updateAnimation 5
        .left:
            updateAnimation 7
        .skipUpdate:
            jmp doMovement

    pacmanAnimation:
        cmp word [pacman_y_speed], -2
        je animationUp
        cmp word [pacman_x_speed], 2
        je animationRight
        cmp word [pacman_y_speed], 2
        je animationDown
        cmp word [pacman_x_speed], -2
        je animationLeft
            pacmanAnimationSide animationUp, 1
            pacmanAnimationSide animationRight, 3
            pacmanAnimationSide animationDown, 5
            pacmanAnimationSide animationLeft, 7