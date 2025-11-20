# IS2021 – Proyecto P1  
**Curso:** Computing Systems  
**Docente:** ladanaque@utec.edu.pe  

---

## 1. Descripción general del proyecto

En un sistema satelital, los datos tomados por un **sensor de temperatura** ubicado en la estructura interna del satélite permiten detectar posibles **malfuncionamientos** a través del calentamiento de los componentes electrónicos.

El objetivo del proyecto es **diseñar e implementar un sistema** que:

- Transmita la medida de temperatura hacia un **receptor** ubicado en el computador a bordo del satélite.
- Sea capaz de **activar una técnica de enfriamiento** (por ejemplo, *shield* o celdas Peltier) cuando la temperatura **supere los 90 °C**.
- Desactive dicha técnica de enfriamiento cuando la temperatura **descienda por debajo de los 55 °C**.

El sistema debe modelarse e implementarse considerando procesos concurrentes y pruebas de desempeño.

---

## 2. Lineamientos generales

- El proyecto se realiza **en grupos**.
- En la **entrega final** se debe adjuntar:
  - Un **informe**.
  - **Todos los archivos de simulación y pruebas** necesarios para mostrar los resultados.
- El proyecto tiene un **checkpoint obligatorio**:
  - Se desarrollará en el **Laboratorio 5**, en la sesión indicada en el Anexo.
  - La modalidad es **presencial**.
- La calificación incluye una **exposición** de todos los miembros del grupo:
  - Se realizará durante la **semana 15**.

---

## 3. Procesos a implementar

Para desarrollar el proyecto se deben implementar **tres procesos** principales:

### 3.1 Proceso 1 – Adquisición de la señal de temperatura

- Se encarga de la **lectura de la temperatura** del sistema.
- Rango de valores de temperatura: **45 °C a 105 °C**.
- La duración de una órbita LEO se considera de alrededor de **100 minutos**  
  (para las pruebas, se permite **acortar este tiempo**).
- Deben **insertarse datos que generen anomalías** (picos de temperatura) para:
  - Provocar la **activación** de las técnicas de enfriamiento.
  - Permitir observar su **posterior desactivación** cuando la temperatura vuelva a valores seguros.

### 3.2 Proceso 2 – Técnica de enfriamiento

- Despliega y controla la **técnica de enfriamiento** (shield, celdas Peltier, etc.).
- Se activa cada vez que la lectura del **Proceso 1** **exceda el valor de 90 °C**.
- Se desactiva cuando la temperatura **desciende por debajo de los 55 °C**.
- Debe mostrar **avisos mediante alertas**:
  - Al **activarse** la técnica de enfriamiento.
  - Al **desactivarse** la técnica de enfriamiento.

### 3.3 Proceso 3 – Recepción UART y despliegue de datos

- Se encarga de **recibir y mostrar** el valor transmitido por el **Proceso 1**.
- La recepción se realiza utilizando un **protocolo serial UART**.
- Debe mostrar el valor recibido de manera clara para poder:
  - Verificar la correcta **transmisión** de datos.
  - Analizar el comportamiento del sistema bajo distintas condiciones.

---

## 4. Consideraciones de diseño

1. Se debe diseñar un **priority scheduler** para recrear **cuatro escenarios** de ejecución (Esc1, Esc2, Esc3, Esc4) de acuerdo con las figuras provistas en el enunciado del curso.
2. **Cada proceso** debe ser implementado en **Assembly RISC-V**, con el fin de:
   - Obtener el valor del **PC (Program Counter)** una vez que se detecte un valor límite de temperatura.
   - Analizar el comportamiento de bajo nivel del sistema durante los eventos críticos (superación de umbrales).

---

## 5. Pruebas y experimentación

Se deben diseñar y ejecutar **pruebas experimentales** para evaluar el desempeño del sistema bajo distintos escenarios y arquitecturas.

### 5.1 Tipos de pruebas sugeridas

Se proponen dos enfoques:

1. **Múltiples archivos de entrada**  
   - Utilizar diversos tipos de archivos de entrada (por ejemplo):
     - Distintos valores de temperatura.
     - Valores aleatorios (*random*).
     - Un mismo valor repetido.
     - 100 minutos de valores.
     - 200 minutos de valores, etc.
   - Probar cada archivo en **todos los escenarios** (Esc1–Esc4).  
     → Si se usan **4 archivos de entrada**, se obtendrían **16 pruebas** (4 archivos × 4 escenarios).

2. **Un solo archivo de entrada**  
   - Utilizar un **solo archivo de entrada** para todos los escenarios.
   - En este caso, se obtendrían **4 pruebas** (una por escenario).

### 5.2 Tabla de pruebas – Versión simple

Ejemplo de formato de tabla de pruebas (métricas agregadas por escenario):

| Metric/Test        | Arch1 Esc1 | Arch1 Esc2 | Arch1 Esc3 | Arch1 Esc4 | Arch2 Esc1 | Arch2 Esc2 | Arch2 Esc3 | Arch2 Esc4 |
|--------------------|-----------:|-----------:|-----------:|-----------:|-----------:|-----------:|-----------:|-----------:|
| Texe               |      0.14  |      0.08  |      0.10  |      0.02  |      0.20  |      0.15  |      0.04  |      0.60  |
| Speedup (Esc1)     | 0.14/0.14  | 0.14/0.08  |     ...    |     ...    |     ...    |     ...    |     ...    |     ...    |
| Syscalls           |           |           |           |           |           |           |           |           |
| Interrupts         |           |           |           |           |           |           |           |           |
| Mem. Occupation    |           |           |           |           |           |           |           |           |
| CPU Occupation     |           |           |           |           |           |           |           |           |
| Insert other metric|           |           |           |           |           |           |           |           |

> **Nota:** Los valores mostrados son solo de ejemplo. Deben ser reemplazados por los resultados obtenidos en sus experimentos.

### 5.3 Tabla de pruebas – Versión detallada por proceso

También se pueden generar tablas que desglosen el tiempo por proceso (**P1, P2, P3**) y el tiempo total (**Ptot**) por escenario y arquitectura:

| Metric/Test    | Arch1 Esc1 P1 | Arch1 Esc1 P2 | Arch1 Esc1 P3 | Arch1 Esc1 Ptot | Arch1 Esc2 P1 | Arch1 Esc2 P2 | Arch1 Esc2 P3 | Arch1 Esc2 Ptot | Arch1 Esc3 P1 | Arch1 Esc3 P2 | Arch1 Esc3 P3 | Arch1 Esc3 Ptot | Arch1 Esc4 P1 | Arch1 Esc4 P2 | Arch1 Esc4 P3 | Arch1 Esc4 Ptot |
|----------------|--------------:|--------------:|--------------:|----------------:|--------------:|--------------:|--------------:|----------------:|--------------:|--------------:|--------------:|----------------:|--------------:|--------------:|--------------:|----------------:|
| Texe           |          0.14 |          0.15 |          0.06 |            0.35 |          0.08 |          0.10 |          0.02 |            0.20 |     (ejemplo) |     (ejemplo) |     (ejemplo) |       (ejemplo) |     (ejemplo) |     (ejemplo) |     (ejemplo) |       (ejemplo) |
| Speedup (Esc1) |             1 |      0.35/0.08|          ...  |            ...  |           ... |           ... |           ... |            ...  |           ... |           ... |           ... |            ...  |           ... |           ... |           ... |            ...  |
| Syscalls       |              X|              X|               |                 |               |               |               |                 |               |               |               |                 |               |               |               |                 |
| Interrupts     |               |               |               |                 |               |               |               |                 |               |               |               |                 |               |               |               |                 |
| Mem. Occupation|               |               |               |                 |               |               |               |                 |               |               |               |                 |               |               |               |                 |
| CPU Occupation |               |               |               |                 |               |               |               |                 |               |               |               |                 |               |               |               |                 |
| Other metric   |               |               |               |                 |               |               |               |                 |               |               |               |                 |               |               |               |                 |

> También se pueden generar **gráficas** con los tiempos de P1, P2, P3 y Ptot, según cada prueba y escenario, para facilitar la comparación visual.

---

## 6. Entregables

En la entrega final del proyecto se debe incluir:

1. **Informe escrito** que contenga:
   - Descripción del problema y del sistema satelital modelado.
   - Diseño de los **tres procesos** (P1, P2, P3).
   - Descripción del **priority scheduler** y explicación de los cuatro escenarios.
   - Detalle de la implementación en **Assembly RISC-V**.
   - Diseño y resultados de las **pruebas** (tablas, gráficas, análisis de resultados).
   - Conclusiones y posibles mejoras del sistema.
2. **Código fuente**:
   - Archivos en **Assembly RISC-V** de cada proceso.
   - Scripts o archivos de configuración utilizados para las pruebas.
3. **Archivos de simulación y pruebas**:
   - Archivos de entrada (valores de temperatura).
   - Resultados obtenidos (logs, salidas, mediciones, etc.).

---

## 7. Resumen rápido de requisitos

- [ ] 3 procesos implementados (lectura temperatura, enfriamiento, visualización UART).  
- [ ] Umbrales correctamente implementados: **> 90 °C activa**, **< 55 °C desactiva**.  
- [ ] Implementación en **Assembly RISC-V**.  
- [ ] **Priority scheduler** con 4 escenarios.  
- [ ] Tabla(s) de pruebas con métricas: tiempo de ejecución, speedup, syscalls, interrupts, memoria, CPU, etc.  
- [ ] Checkpoint cumplido en Laboratorio 5 (presencial).  
- [ ] Exposición de todos los integrantes en semana 15.  
- [ ] Entrega de informe + código + archivos de simulación/pruebas.  

---