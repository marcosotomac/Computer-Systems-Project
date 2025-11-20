# Escenario 2 – Prioridad impuesta con conmutación y detección de pérdidas

### Proyecto Final – Sistemas Computacionales

**Profesor:** Luz A. Adanaqué  
**Alumnos:** _Marco Soto, Joseph Cabanillas, Gussephe Benjamin_

---

## Descripción general

El Escenario 2 representa la simulación de un _scheduler_ con **prioridades impuestas** en un sistema operativo para un satélite.  
Los procesos ya no se ejecutan de manera secuencial como en el escenario anterior: ahora existe una **jerarquía de prioridad fija (P1 > P3 > P2)**, y el sistema debe ser capaz de **interrumpir procesos**, registrar los cambios y **detectar pérdidas de información** cuando la conmutación ocurre entre procesos no consecutivos.

---

## Objetivos principales

- Implementar un _scheduler preemptivo_ con prioridad fija.
- Detectar y reportar pérdidas de información cuando hay cambios abruptos entre procesos no consecutivos.
- Mantener el mismo ciclo orbital de 100 minutos (42 min zona luminosa y 58 min zona oscura).
- Simular el guardado de _Program Counter (PC)_ en cada cambio de proceso.

---

## Procesos

| Proceso                    | Descripción                                               | Condición   |
| -------------------------- | --------------------------------------------------------- | ----------- |
| **P1 – Sensor**            | Lee temperatura determinística (45–105 °C) desde el dataset cargado por zona orbital. | Cada 5 min  |
| **P2 – Enfriamiento**      | Se activa si T>90 °C y se apaga si T<60 °C.               | Condicional |
| **P3 – Comunicación UART** | Transmite la lectura de temperatura y estado del sistema. | Continuo    |

**Orden de prioridad:** `P1 > P3 > P2`

---

## Lógica de funcionamiento

1. **Carga de dataset:** Antes de iniciar la órbita se carga uno de los cuatro archivos `../data/dataset_case*.txt`, cada uno con 20 muestras (5 min c/u) que incluyen valores anómalos.
2. **Prioridades impuestas:** El OS ejecuta los procesos según el orden definido.
3. **Eventos anómalos:** Si P1 registra una temperatura ≥100 °C, se fuerza un salto inmediato a P2 (no consecutivo).
4. **Cambio de contexto:** Se guarda el _program counter_ y se registra la transición.
5. **Pérdidas de información:** Si el proceso interrumpido estaba con datos sin enviar o sin registrar (`dirty`), se contabiliza una pérdida simulada (bytes UART o muestras no consumidas).
6. **Reporte:** El sistema imprime en consola los procesos activos, los cambios de contexto y un resumen final con métricas del scheduler.

---

## Diagrama de flujo

```mermaid
flowchart TD
    A([Inicio del sistema]) --> B[Leer temperatura P1]
    B --> C{Temperatura mayor o igual a 100C}
    C -->|Si| D[Salto a P2 cambio abrupto]
    C -->|No| E[Continuar segun prioridad]
    E --> F[Ejecutar P3 UART]
    D --> G[Registrar perdida de datos]
    F --> H[Actualizar PC y cambiar proceso]
    G --> H
    H --> I[Incrementar tiempo mas 5 min]
    I -->|Menor a 100 min| B
    I -->|Fin del ciclo| J([Resumen final])
```

---

## Ejecución del programa

```bash
cd scenario2
./compile.sh

# Ejecutar con Spike + pk y un dataset (ejemplo: case2)
spike --isa=rv64imac \
  /opt/homebrew/opt/riscv-pk/riscv64-unknown-elf/bin/pk \
  programa ../data/dataset_case2.txt
```

- `compile.sh` ensambla `P1.s`, `P2.s` y `P3.s` en RV64IMAC y enlaza el scheduler.
- Los datasets `../data/dataset_case{1..4}.txt` replican distintos perfiles térmicos: `case2` provoca múltiples anomalías, `case3` mantiene el enfriamiento apagado en la órbita nocturna, etc.

---

## Ejemplo de salida

```
=== ESCENARIO 2: Prioridad impuesta (P1 > P3 > P2) ===
Dataset cargado: ../data/dataset_case2.txt (20 muestras)

⏱️  t=0 min | Zona=Luminosa
[P1] t=  0 min | Temp=98 C | Zona=Luminosa | pc=1
↔️  Cambio de contexto P1 -> P3
[OS] UART=0B pend | Cooling=OFF

⏱️  t=5 min | Zona=Luminosa
[P3] UART Transmission:
 - Temp: 98 C
 - Cooling: 0
 - Zone: 1 (1=luz,0=oscuridad)
[P3] TX 16B (pendiente= 6B) | pc=1
↔️  Cambio de contexto P3 -> P2
[OS] UART=6B pend | Cooling=OFF

⏱️  t=15 min | Zona=Luminosa
[P1] t= 15 min | Temp=101 C | Zona=Luminosa | pc=2
🔁 Cambio ABRUPTO P1 -> P2 | pérdida=4B
[OS] UART=6B pend | Cooling=ON
...

===== RESUMEN =====
Context switches: 20 | Abruptos: 4
Pérdidas (B): P1=16, P3=0, P2=0
```

---

## Observaciones técnicas

- Las conmutaciones se registran con indicador visual (`↔️` normal, `🔁` abrupto).
- Los procesos (`P1`, `P2`, `P3`) están escritos en ensamblador RISC‑V y se comparten con el Escenario 1; el scheduler en C controla su ejecución y contexto.
- Las pérdidas simuladas se basan en datos no transmitidos o muestras no guardadas durante un cambio abrupto.
- El sistema mantiene la órbita de 100 min (42 min de luz) y permite repetir pruebas cambiando únicamente el dataset.

---

## Referencias

- Patterson, D. A., & Hennessy, J. L. (2020). _Computer Organization and Design, RISC‑V Edition._ Morgan Kaufmann.
- Silberschatz, A., Galvin, P. B., & Gagne, G. (2015). _Operating System Concepts._ Wiley.
- Harris, S., & Harris, D. (2021). _Digital Design and Computer Architecture, RISC‑V Edition._ Morgan Kaufmann.
