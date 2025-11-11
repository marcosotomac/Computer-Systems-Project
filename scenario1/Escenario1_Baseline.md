---
title: "Escenario 1 – Baseline"
author: "[tu nombre]"
format: html
---

# 🛰️ Escenario 1 – Baseline  
### Proyecto Final – Sistemas Computacionales  
**Profesor:** Luz A. Adanaqué  
**Alumno:** *[tu nombre]*  
**Tema:** Implementación de procesos y scheduler en un entorno RISC-V (simulación satelital)

---

## 📋 Descripción general

El **Escenario 1 (Baseline)** representa la **fase inicial de prueba del scheduler** dentro del sistema operativo que controla tres procesos relacionados con el sensado remoto de un satélite en órbita baja (LEO).  

En este escenario, los procesos **se ejecutan secuencialmente** y **sin prioridades**. El sistema operativo debe ser capaz de reconocer qué proceso está en ejecución, pero **sin interrupciones ni llamadas al sistema (syscalls)**.

---

## ⚙️ Objetivo

Simular un ciclo orbital de 100 minutos en el que se miden parámetros térmicos del satélite, se aplican técnicas de enfriamiento cuando sea necesario y se transmiten los datos obtenidos por medio de una interfaz UART.

---

## 🧩 Procesos implementados

| Proceso | Nombre | Descripción | Condición de ejecución |
|----------|---------|-------------|------------------------|
| **P1** | Sensor de temperatura | Captura datos de temperatura (45 °C – 105 °C) diferenciando entre zona luminosa y oscura de la órbita. | Cada 5 min |
| **P2** | Enfriamiento | Activa el sistema de enfriamiento si la temperatura supera 90 °C y lo desactiva al descender por debajo de 60 °C. | Condicional |
| **P3** | Comunicación UART | Muestra los datos generados por el sensor (P1) y el estado del enfriamiento (P2). | Continuamente |

---

## 🛰️ Contexto orbital

El satélite describe una órbita baja (LEO) de **100 min**, dividida en:

- **Zona luminosa:** 42 min  
- **Zona oscura:** 58 min  

Durante la zona luminosa, la temperatura del satélite tiende a aumentar; en la zona oscura, a disminuir. Esto genera condiciones variables que deben ser gestionadas por el sistema operativo.

---

## 🧠 Lógica de ejecución

1. **Inicio del ciclo orbital:**  
   El sistema inicializa los procesos y el tiempo simulado.  
2. **Lectura del sensor (P1):**  
   Se genera un valor de temperatura aleatorio según la zona (luminosa u oscura).  
3. **Verificación térmica (P2):**  
   El proceso de enfriamiento revisa la temperatura leída:  
   - Si > 90 °C → activa el sistema y muestra alerta.  
   - Si < 60 °C → desactiva el sistema.  
4. **Transmisión de datos (P3):**  
   Muestra la temperatura actual y el estado del enfriamiento vía UART (simulada en consola).  
5. **Scheduler secuencial:**  
   Los procesos se ejecutan de forma ordenada (**P1 → P2 → P3**) en intervalos de 5 min.  
   No hay prioridades ni interrupciones.  
6. **Fin del ciclo:**  
   Tras 100 min, el sistema concluye la simulación y muestra el cierre del escenario.

---

## 🧮 Diagrama de flujo simplificado

```{mermaid}
flowchart TD
    A([Inicio del sistema]) --> B[Zona orbital detectada]
    B --> C[P1: Sensor lee temperatura]
    C --> D[P2: Verifica temperatura]
    D --> E{Temperatura}
    E -->|> 90C| F[Activa enfriamiento]
    E -->|< 60C| G[Desactiva enfriamiento]
    F --> H[P3: Transmite datos UART]
    G --> H
    H --> I[Incrementa tiempo (+5 min)]
    I -->|Menor a 100 min| B
    I -->|Fin del ciclo| J([Fin del ciclo orbital])
```

---

## 💻 Ejecución del sistema

```bash
gcc escenario1.c -o escenario1
./escenario1
```

O, para entorno RISC-V:

```bash
riscv64-unknown-elf-gcc -o escenario1 escenario1.c
spike pk escenario1
```

---

## 📈 Ejemplo de salida

```
🌍 INICIO DE SIMULACIÓN - ESCENARIO 1 (BASELINE)
Duración de la órbita: 100 min (Luminosa: 42, Oscura: 58)
--------------------------------------------------

⏱️  Tiempo simulado: 0 min
[P1] Temperatura leída: 95°C (Zona luminosa)
[P2] ⚠️  Enfriamiento ACTIVADO (T > 90°C)
[P3] UART >> Transmisión de datos:
     Temperatura actual: 95°C | Enfriamiento: ON
--------------------------------------------------

⏱️  Tiempo simulado: 10 min
[P1] Temperatura leída: 58°C (Zona luminosa)
[P2] ✅  Enfriamiento DESACTIVADO (T < 60°C)
[P3] UART >> Transmisión de datos:
     Temperatura actual: 58°C | Enfriamiento: OFF
--------------------------------------------------
```

---

## 🧠 Consideraciones técnicas

- Cada proceso simula su propio contexto (estado y variables compartidas).  
- El scheduler no implementa cambio de contexto ni prioridades.  
- Las zonas orbitales influyen en el comportamiento térmico.  
- Las funciones están listas para integrarse con **interrupciones y syscalls** en escenarios posteriores.  

---

## 📚 Referencias bibliográficas

- Patterson, D. A., & Hennessy, J. L. (2020). *Computer Organization and Design, RISC-V Edition.* Morgan Kaufmann.  
- Silberschatz, A., Galvin, P. B., & Gagne, G. (2015). *Operating System Concepts (9th ed.).* Wiley.  
- Harris, S., & Harris, D. (2021). *Digital Design and Computer Architecture, RISC-V Edition.* Morgan Kaufmann.  
- Nisan, N., & Schocken, S. (2021). *The Elements of Computing Systems.* MIT Press.
