.section .text
    .globl P1

# P1(int zone)  -> retorna temperatura en a0
# zone: 1 = luminosa, 0 = oscura

P1:
    # Guardar zona en t0
    mv t0, a0

    # Usaremos un LCG para pseudoaleatorio simple
    # seed = seed * 1103515245 + 12345
    la t1, seed
    lw t2, 0(t1)
    li t3, 1103515245
    mul t2, t2, t3
    
    # Para sumar 12345, necesitamos usar li primero
    li t4, 12345
    add t2, t2, t4
    sw t2, 0(t1)

    # Seleccionar rango según zona
    beq t0, zero, zona_oscura

zona_luminosa:
    # temperatura = 85 + (seed % 20)
    li t4, 20
    rem t5, t2, t4
    addi a0, t5, 85
    ret

zona_oscura:
    # temperatura = 45 + (seed % 30)
    li t4, 30
    rem t5, t2, t4
    addi a0, t5, 45
    ret

    .section .data
seed:
    .word 1234567