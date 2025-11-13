    .section .text
    .globl P3

# P3(int temp, int cooling, int zone)

P3:
    # temp = a0
    # cooling = a1
    # zone = a2

    # Llamamos a printf desde ensamblador

    # Imprimir encabezado
    la a0, msg_header
    call printf

    # Imprimir temperatura
    la a0, msg_temp
    mv a1, a3       # a3 no está usado: mover temp
    mv a1, t0
    mv t0, a0
    mv a0, msg_temp
    mv a1, t0
    call printf

    # Imprimir cooling
    mv t0, a1
    la a0, msg_cool
    mv a1, t0
    call printf

    # Imprimir zona
    mv t0, a2
    la a0, msg_zone
    mv a1, t0
    call printf

    ret

    .section .rodata
msg_header:
    .asciz "[P3] UART Transmission:\n"
msg_temp:
    .asciz " - Temp: %d C\n"
msg_cool:
    .asciz " - Cooling: %d\n"
msg_zone:
    .asciz " - Zone: %d (1=luz,0=oscuridad)\n"