#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   // para sleep()
#include <time.h>     // para srand y rand()

// -------------------- VARIABLES GLOBALES --------------------
int temperature = 0;    // temperatura actual
int cooling = 0;        // 0 = apagado, 1 = encendido
int minute = 0;         // tiempo simulado en minutos

// Constantes del sistema
#define ORBIT_TIME 100      // duración total de la órbita
#define BRIGHT_ZONE 42      // minutos de zona luminosa
#define DARK_ZONE 58        // minutos de zona oscura
#define STEP 5              // intervalo de sensado (minutos)

// -------------------- PROCESO 1: SENSOR --------------------
void P1_sensor() {
    // Simulación distinta según la zona
    if (minute < BRIGHT_ZONE) {
        // Zona luminosa: mayor temperatura
        temperature = 70 + rand() % 36; // 70–105°C
    } else {
        // Zona oscura: menor temperatura
        temperature = 45 + rand() % 31; // 45–75°C
    }

    printf("[P1] Temperatura leída: %d°C (%s)\n",
           temperature,
           (minute < BRIGHT_ZONE) ? "Zona luminosa" : "Zona oscura");
}

// -------------------- PROCESO 2: ENFRIAMIENTO --------------------
void P2_cooling() {
    if (temperature > 90 && !cooling) {
        cooling = 1;
        printf("[P2] ⚠️  Enfriamiento ACTIVADO (T > 90°C)\n");
    } else if (temperature < 60 && cooling) {
        cooling = 0;
        printf("[P2] ✅  Enfriamiento DESACTIVADO (T < 60°C)\n");
    } else {
        printf("[P2] Estado sin cambios. Enfriamiento: %s\n",
               cooling ? "ON" : "OFF");
    }
}

// -------------------- PROCESO 3: COMUNICACIÓN UART --------------------
void P3_uart() {
    printf("[P3] UART >> Transmisión de datos:\n");
    printf("     Temperatura actual: %d°C | Enfriamiento: %s\n",
           temperature,
           cooling ? "ON" : "OFF");
    printf("--------------------------------------------------\n");
}

// -------------------- FUNCIÓN PRINCIPAL (SCHEDULER BASELINE) --------------------
int main() {
    srand(time(NULL));  // inicializa generador aleatorio

    printf("🌍 INICIO DE SIMULACIÓN - ESCENARIO 1 (BASELINE)\n");
    printf("Duración de la órbita: %d min (Luminosa: %d, Oscura: %d)\n",
           ORBIT_TIME, BRIGHT_ZONE, DARK_ZONE);
    printf("--------------------------------------------------\n\n");

    for (minute = 0; minute <= ORBIT_TIME; minute += STEP) {
        printf("⏱️  Tiempo simulado: %d min\n", minute);

        // Ejecución secuencial (sin prioridad)
        P1_sensor();
        P2_cooling();
        P3_uart();

        sleep(1);  // simula paso del tiempo (1s = 5min)
    }

    printf("\n✅ Fin de simulación del Escenario 1.\n");
    return 0;
}