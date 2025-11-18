.section .text
    .globl P3

# P3(int temp, int cooling, int zone)
# a0 = temp
# a1 = cooling
# a2 = zone

P3:
    # Guardar registros en el stack
    addi sp, sp, -16
    sw ra, 12(sp)      # Guardar return address
    sw a0, 8(sp)       # Guardar temp
    sw a1, 4(sp)       # Guardar cooling
    sw a2, 0(sp)       # Guardar zone

    # Imprimir encabezado
    la a0, msg_header
    call printf

    # Imprimir temperatura
    lw a1, 8(sp)       # Recuperar temp en a1
    la a0, msg_temp
    call printf

    # Imprimir cooling
    lw a1, 4(sp)       # Recuperar cooling en a1
    la a0, msg_cool
    call printf

    # Imprimir zona
    lw a1, 0(sp)       # Recuperar zone en a1
    la a0, msg_zone
    call printf

    # Restaurar registros y retornar
    lw ra, 12(sp)
    lw a0, 8(sp)
    lw a1, 4(sp)
    lw a2, 0(sp)
    addi sp, sp, 16
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