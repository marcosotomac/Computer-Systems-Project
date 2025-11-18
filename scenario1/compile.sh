#!/bin/bash

# Script para compilar proyecto RISC-V64 en macOS ARM (Spike + pk)

set -e

ARCH="rv64imac"
ABI="lp64"
ISA_FLAGS="-march=${ARCH} -mabi=${ABI}"

echo "=== Compilando proyecto RISC-V64 (${ARCH}/${ABI}) ==="

# Verificar que existen los archivos
for file in main.c P1.s P2.s P3.s; do
    if [ ! -f "$file" ]; then
        echo "Error: No se encuentra $file"
        exit 1
    fi
done

# Limpiar archivos anteriores
rm -f *.o programa

echo "1. Compilando archivos assembly..."
riscv64-unknown-elf-as $ISA_FLAGS P1.s -o P1.o
riscv64-unknown-elf-as $ISA_FLAGS P2.s -o P2.o
riscv64-unknown-elf-as $ISA_FLAGS P3.s -o P3.o

echo "2. Compilando main.c..."
riscv64-unknown-elf-gcc $ISA_FLAGS -c main.c -o main.o

echo "3. Enlazando todo..."
riscv64-unknown-elf-gcc $ISA_FLAGS main.o P1.o P2.o P3.o -o programa

echo ""
echo "✓ Compilación exitosa!"
echo "Ejecutable generado: programa"
echo ""
PK_PREFIX="$(brew --prefix riscv-pk 2>/dev/null || true)"
PK_BIN="${PK_PREFIX}/riscv64-unknown-elf/bin/pk"
if [ -x "$PK_BIN" ]; then
    echo "Para ejecutar con Spike y seleccionar un dataset:"
    echo "  spike --isa=${ARCH} ${PK_BIN} programa data/dataset_case1.txt"
    echo "  spike --isa=${ARCH} ${PK_BIN} programa data/dataset_case2.txt"
    echo "  spike --isa=${ARCH} ${PK_BIN} programa data/dataset_case3.txt"
    echo "  spike --isa=${ARCH} ${PK_BIN} programa data/dataset_case4.txt"
else
    echo "Instala riscv-pk para poder ejecutar con Spike:"
    echo "  brew install riscv-software-src/riscv/riscv-pk"
fi
