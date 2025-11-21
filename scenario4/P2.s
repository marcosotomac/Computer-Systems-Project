.section .text
    .globl P2
    .globl P2_get_pc

# P2(int temp) -> retorna 1 (ON) o 0 (OFF)

P2:
    # pc simulado (contador de ejecuciones)
    la t3, p2_pc
    lw t4, 0(t3)
    addi t4, t4, 1
    sw t4, 0(t3)

    mv t0, a0          # temp

    # Si temp > 90 → activar
    li t1, 90
    bgt t0, t1, activar

    # Si temp < 60 → desactivar
    li t1, 60
    blt t0, t1, desactivar

    # Mantener estado → devolver lo mismo que estaba guardado
    la t2, cooling_state
    lw a0, 0(t2)
    ret

activar:
    li a0, 1
    la t2, cooling_state
    sw a0, 0(t2)
    ret

desactivar:
    li a0, 0
    la t2, cooling_state
    sw a0, 0(t2)
    ret

# int P2_get_pc(void) -> pc simulado (contador de toggles)
P2_get_pc:
    la t0, p2_pc
    lw a0, 0(t0)
    ret

    .section .data
cooling_state:
    .word 0

p2_pc:
    .word 0
