#!/bin/bash

set -e

ARCH="rv64imac"
ABI="lp64"
ISA_FLAGS="-march=${ARCH} -mabi=${ABI}"
OUTPUT="programa"

echo "=== Compilando Escenario 4 (Syscalls) RV64: ${ARCH}/${ABI}) ==="

for file in main.c P1.s P2.s P3.s; do
    if [ ! -f "$file" ]; then
        echo "Error: falta $file en $(pwd)"
        exit 1
    fi
done

rm -f *.o "$OUTPUT"

echo "1. Ensamblando procesos..."
riscv64-unknown-elf-as $ISA_FLAGS P1.s -o P1.o
riscv64-unknown-elf-as $ISA_FLAGS P2.s -o P2.o
riscv64-unknown-elf-as $ISA_FLAGS P3.s -o P3.o

echo "2. Compilando scheduler..."
riscv64-unknown-elf-gcc $ISA_FLAGS -O2 -c main.c -o main.o

echo "3. Enlazando todo..."
riscv64-unknown-elf-gcc $ISA_FLAGS main.o P1.o P2.o P3.o -o "$OUTPUT"

echo "✓ Compilación completada: $OUTPUT"
PK_PREFIX="$(brew --prefix riscv-pk 2>/dev/null || true)"
PK_BIN="${PK_PREFIX}/riscv64-unknown-elf/bin/pk"
if [ -x "$PK_BIN" ]; then
    echo "Para ejecutar (elige dataset en ../data):"
    echo "  spike --isa=${ARCH} ${PK_BIN} $OUTPUT ../data/dataset_case1.txt"
    echo "  spike --isa=${ARCH} ${PK_BIN} $OUTPUT ../data/dataset_case2.txt"
    echo "  spike --isa=${ARCH} ${PK_BIN} $OUTPUT ../data/dataset_case3.txt"
    echo "  spike --isa=${ARCH} ${PK_BIN} $OUTPUT ../data/dataset_case4.txt"
else
    echo "Instala riscv-pk para ejecutar con Spike:"
    echo "  brew install riscv-software-src/riscv/riscv-pk"
fi
