# Escenario 3 – Priorización invertida y reacción inmediata

### Proyecto Final – Sistemas Computacionales

**Profesor:** Luz A. Adanaqué  
**Alumnos:** _Marco Soto, Joseph Cabanillas, Gussephe Benjamin_

---

## Descripción general

El Escenario 3 continúa la simulación del _scheduler_ satelital agregando una **prioridad invertida**: ahora el sistema favorece al proceso de enfriamiento (`P2`) antes de consultar el sensor o transmitir datos.  
Esta variante busca evaluar qué tan rápido se pueden aplicar técnicas de enfriamiento cuando hay lecturas críticas y cómo impacta ese cambio en las pérdidas por conmutaciones abruptas.

---

## Objetivos principales

- Reordenar el _scheduler preemptivo_ para imponer la prioridad fija `P2 > P1 > P3`.
- Reaccionar al instante cuando `P1` detecta valores ≥100 °C saltando a `P2`, registrando el cambio abrupto.
- Mantener el ciclo orbital de 100 minutos (42 min luz / 58 min oscuridad) y el registro del PC en cada cambio.
- Cuantificar el impacto del nuevo orden en cambios de contexto, pérdidas de datos y métricas pedidas en el informe.

---

## Procesos

| Proceso                    | Descripción                                               | Condición   |
| -------------------------- | --------------------------------------------------------- | ----------- |
| **P1 – Sensor**            | Lee temperatura determinística (45–105 °C) desde el dataset cargado por zona orbital. | Cada 5 min  |
| **P2 – Enfriamiento**      | Tiene prioridad máxima; se activa con T>90 °C y se apaga si T<60 °C. | Condicional |
| **P3 – Comunicación UART** | Transmite la lectura de temperatura y estado del sistema. | Continuo    |

**Orden de prioridad:** `P2 > P1 > P3`

---

## Lógica de funcionamiento

1. **Carga de dataset:** Igual que en los demás escenarios, se cargan 20 muestras determinísticas desde `../data/dataset_case*.txt` (se mantienen como archivos de texto, sin embebido).
2. **Prioridades invertidas:** El OS ejecuta los procesos siguiendo `P2 → P1 → P3`. `P2` puede ejecutarse incluso con la última temperatura conocida mientras espera una nueva lectura.
3. **Eventos anómalos:** Si `P1` registra una temperatura ≥100 °C, se produce un salto inmediato (no consecutivo) hacia `P2` para aplicar cooling.
4. **Cambio de contexto:** Se guarda el _program counter_ de cada proceso y se notifica si el salto fue normal (`↔️`) o abrupto (`🔁`).
5. **Pérdidas de información:** Los procesos `dirty` generan pérdidas simuladas (bytes UART o muestras no procesadas) cuando el cambio es abrupto.
6. **Reporte:** El scheduler imprime los procesos ejecutados, el estado del buffer UART / cooling y un resumen con métricas.

---

## Diagrama de flujo

```mermaid
flowchart TD
    A([Inicio del sistema]) --> B[Leer temperatura P1]
    B --> C{Temperatura mayor o igual a 100C}
    C -->|Si| D[Salto a P2 cambio abrupto]
    C -->|No| E[Continuar con prioridad invertida (P2>P1>P3)]
    E --> F[Ejecutar UART o siguiente proceso]
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
cd scenario3
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
=== ESCENARIO 3: Priorización invertida (P2 > P1 > P3) ===
Dataset cargado: ../data/dataset_case2.txt (20 muestras)

⏱️  t=0 min | Zona=Luminosa
[P2] Estado=OFF | pc=1
↔️  Cambio de contexto P2 -> P1
[OS] UART=0B pend | Cooling=OFF

⏱️  t=5 min | Zona=Luminosa
[P1] t=  5 min | Temp=98 C | Zona=Luminosa | pc=1
↔️  Cambio de contexto P1 -> P3
[OS] UART=0B pend | Cooling=OFF

⏱️  t=10 min | Zona=Luminosa
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
Pérdidas (B): P1=16, P3=8, P2=0
```

---

## Observaciones técnicas

- Las conmutaciones se registran con indicador visual (`↔️` normal, `🔁` abrupto).
- Los procesos (`P1`, `P2`, `P3`) están escritos en ensamblador RISC‑V y se comparten con el Escenario 1; el scheduler en C controla su ejecución y contexto.
- Las pérdidas simuladas se basan en datos no transmitidos o muestras no guardadas durante un cambio abrupto.
- El sistema mantiene la órbita de 100 min (42 min de luz) y permite repetir pruebas cambiando únicamente el dataset.

---

## Métricas reportadas

Siguiendo las recomendaciones del enunciado (`IS2021_ProyectoP1.md`) y del README general, el escenario 3 también imprime automáticamente un bloque de métricas al finalizar la simulación:

| Métrica                          | Descripción                                                                                                  |
| -------------------------------- | ------------------------------------------------------------------------------------------------------------ |
| `Texe total`                     | Tiempo de pared de toda la órbita (100 min simulados).                                                       |
| `Interrupciones por anomalías`   | Número de saltos forzados por lecturas mayores o iguales a 100 °C.                                           |
| `Proceso | Tiempo total / Prom.` | Tiempo acumulado y promedio de cada proceso (`P2`, `P1`, `P3`), más el _speedup_ relativo contra el más lento. |
| `CPU Occupation`                 | Porcentaje de uso del CPU simulado, comparando tiempo activo de procesos vs. `Texe`.                        |
| `Mem. Occupation`                | Huella aproximada en KB de buffers, métricas y PCB durante la corrida.                                      |

No se emulan syscalls en el scheduler; la interacción se limita a leer los datasets `.txt` y a registrar la actividad de los procesos.

Estas métricas permiten contrastar este escenario con el baseline (Esc. 1) y con el de prioridades originales (Esc. 2), preparando tablas comparativas que muestren el impacto del nuevo orden.

---

## Referencias

- Patterson, D. A., & Hennessy, J. L. (2020). _Computer Organization and Design, RISC‑V Edition._ Morgan Kaufmann.
- Silberschatz, A., Galvin, P. B., & Gagne, G. (2015). _Operating System Concepts._ Wiley.
- Harris, S., & Harris, D. (2021). _Digital Design and Computer Architecture, RISC‑V Edition._ Morgan Kaufmann.
