# Proyecto Final – Sistemas Computacionales

Simulación en RISC-V de procesos satelitales orientada a estudiar diferentes estrategias de planificación. El repositorio contiene dos escenarios ejecutables sobre Spike + Proxy Kernel (`pk`) usando un toolchain `riscv64-unknown-elf`. Ambos escenarios reutilizan los mismos procesos escritos en ensamblador (sensor, enfriamiento y comunicación), pero difieren en la lógica del scheduler implementada en C.

## Requisitos

- macOS con Homebrew.
- Toolchain `riscv64-unknown-elf` (`brew install riscv64-elf-gcc`, incluido con `riscv-gnu-toolchain`).
- Spike + Proxy Kernel (`brew install riscv-software-src/riscv/riscv-isa-sim` y `.../riscv-pk`).
- Opcional: QEMU para ejecutar binarios RV32 (no se usa en los escenarios actuales).

Verifica que `riscv64-unknown-elf-gcc`, `riscv64-unknown-elf-as`, `spike` y `pk` estén en el `PATH`.

## Estructura del repositorio

```
scenario1/
  main.c
  P1.s P2.s P3.s
  compile.sh
scenario2/
  main.c
  P1.s P2.s P3.s
  compile.sh
data/
  dataset_case{1..4}.txt
```

En ambos escenarios los procesos están en ensamblador RISC-V (RV64IMAC) y se enlazan con un `main.c` específico.

## Escenario 1 – Baseline

### Descripción

Simula una órbita LEO de 100 minutos dividida en 42 minutos de zona luminosa y 58 minutos de zona oscura. Los procesos se ejecutan secuencialmente sin prioridades ni interrupciones:

1. **P1 – Sensor de temperatura:** lee una muestra determinística (45–105 °C) desde un dataset específico por zona orbital.
2. **P2 – Enfriamiento:** activa el cooling cuando la temperatura supera 90 °C y lo desactiva al bajar de 60 °C.
3. **P3 – Comunicación UART:** imprime por consola la temperatura y el estado del cooling.

Los datasets (`data/dataset_case*.txt`) contienen 20 muestras (5 min cada una) e incluyen valores anómalos para forzar la activación/desactivación del enfriamiento.

### Compilación y ejecución

```bash
cd scenario1
./compile.sh

# Ejecutar con Spike + pk
spike --isa=rv64imac \
  /opt/homebrew/opt/riscv-pk/riscv64-unknown-elf/bin/pk \
  programa ../data/dataset_case2.txt
```

El script `compile.sh` ensambla `P1/P2/P3` y compila `main.c` con `riscv64-unknown-elf-gcc`. Al ejecutar `programa` con Spike se puede elegir cualquiera de los cuatro datasets de prueba.

### Resultados esperados

El sistema imprime un bloque por cada intervalo de 5 minutos, mostrando la zona (luz/oscuridad), la temperatura leída, el estado del cooling y la transmisión UART. El enfriamiento responde únicamente a los valores del dataset por lo que la ejecución es determinística.

## Escenario 2 – Prioridades y detección de pérdidas

### Descripción

Extiende los procesos anteriores a un scheduler con prioridad fija `P1 > P3 > P2` y cambios de contexto controlados. Características relevantes:

- **Datasets determinísticos:** se cargan los mismos archivos `data/dataset_case*.txt` (ubicados en la raíz del repositorio) para que P1 produzca lecturas reproducibles.
- **Detección de anomalías:** cuando P1 reporta temperaturas ≥100 °C se fuerza un salto inmediato a P2 (cambio abrupto).
- **Context switching:** se guardan contadores de programa, se contabilizan pérdidas si el proceso interrumpido tenía datos sin transmitir (`dirty`) y se distinguen conmutaciones normales (`↔️`) de abruptas (`🔁`).
- **Reporte final:** incluye número de cambios de contexto totales/abruptos y bytes perdidos por proceso.

### Compilación y ejecución

```bash
cd scenario2
./compile.sh

spike --isa=rv64imac \
  /opt/homebrew/opt/riscv-pk/riscv64-unknown-elf/bin/pk \
  programa ../data/dataset_case2.txt
```

`compile.sh` ensambla los tres procesos y enlaza el scheduler (`main.c`). Cambiando el dataset se pueden reproducir distintos comportamientos: por ejemplo, `dataset_case2` introduce múltiples anomalías consecutivas, mientras que `dataset_case3` forza una rampa descendente en la zona oscura.

### Resultados esperados

La salida muestra, para cada intervalo, qué proceso se ejecutó, el estado del buffer UART y los cambios de contexto. El resumen final reseña los cambios totales y las pérdidas acumuladas por proceso.

## Escenario 3 – Priorización invertida

### Descripción

Deriva del escenario anterior, pero invierte la prioridad fija del scheduler para favorecer al proceso de enfriamiento (`P2 > P1 > P3`). Así se puede evaluar qué tan rápido se aplican las técnicas de cooling ante valores anómalos (≥100 °C) y cómo impacta eso en los bytes perdidos cuando el salto se produce entre procesos no consecutivos.

### Compilación y ejecución

```bash
cd scenario3
./compile.sh

spike --isa=rv64imac \
  /opt/homebrew/opt/riscv-pk/riscv64-unknown-elf/bin/pk \
  programa ../data/dataset_case2.txt
```

`compile.sh` ensambla los mismos procesos en ensamblador y enlaza un scheduler que contabiliza los context switches, las pérdidas y un bloque adicional de métricas (Texe, syscalls simuladas, interrupciones por anomalías, promedios por proceso, ocupación de CPU/memoria) para comparar con los escenarios 1 y 2.

## Datasets disponibles

| Dataset               | Descripción resumida                                                                      |
|-----------------------|-------------------------------------------------------------------------------------------|
| `dataset_case1.txt`   | Perfil mixto; activa cooling al final de la fase luminosa y lo libera al inicio de la oscura. |
| `dataset_case2.txt`   | Varias muestras ≥100 °C para evaluar cambios abruptos reiterados y cooling sostenido.      |
| `dataset_case3.txt`   | Descenso progresivo en zona oscura, manteniendo el cooling apagado en la segunda mitad.   |
| `dataset_case4.txt`   | Combinación de picos en luz y oscilaciones en oscuridad para evaluar reacciones frecuentes. |

Cada dataset contiene **100 muestras** (equivalentes a cinco órbitas de 5 minutos por lectura); los escenarios consumen al menos las primeras 20, pero se pueden alargar las simulaciones o repetirlas desplazando el índice para analizar comportamientos más complejos.

## Próximos pasos

- Incorporar nuevos escenarios con prioridades dinámicas e interrupciones reales.
- Integrar pruebas automatizadas que comparen la salida con logs esperados por dataset.
- Documentar resultados cuantitativos (gráficas de temperatura, consumo energético estimado) para cada caso.
