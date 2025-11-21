.section .text
    .globl P1
    .globl P1_set_dataset
    .globl P1_get_pc

# P1(int zone) -> retorna la siguiente muestra configurada

P1:
    la t0, dataset_ptr
    ld t1, 0(t0)          # t1 = puntero al arreglo
    beqz t1, default_temp

    la t2, dataset_len
    lw t3, 0(t2)          # t3 = longitud
    blez t3, default_temp

    la t4, dataset_idx
    lw t5, 0(t4)          # t5 = índice actual
    bge t5, t3, use_last

    slli t6, t5, 2        # offset = indice * sizeof(int)
    add t6, t1, t6
    lw a0, 0(t6)

    addi t5, t5, 1        # avanzar índice
    sw t5, 0(t4)
    ret

use_last:
    addi t6, t3, -1
    blt t6, zero, default_temp
    slli t6, t6, 2
    add t6, t1, t6
    lw a0, 0(t6)
    ret

default_temp:
    li a0, 70
    ret

# void P1_set_dataset(const int *data, int length)
P1_set_dataset:
    la t0, dataset_ptr
    sd a0, 0(t0)

    la t0, dataset_len
    sw a1, 0(t0)

    la t0, dataset_idx
    sw zero, 0(t0)
    ret

# int P1_get_pc(void) -> devuelve el pc simulado (indice actual)
P1_get_pc:
    la t0, dataset_idx
    lw a0, 0(t0)
    ret

    .section .bss
    .align 3
dataset_ptr:
    .zero 8

    .align 2
dataset_len:
    .zero 4

dataset_idx:
    .zero 4
