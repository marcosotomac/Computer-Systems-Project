#include <stdio.h>

extern int P1(int zone);
extern int P2(int temp);
extern void P3(int temp, int cooling, int zone);

int main() {
    int tiempo = 0;
    int zona = 1;   // 1=luminosa, 0=oscura

    printf("===== ESCENARIO 1 – BASELINE =====\n");

    while (tiempo < 100) {

        if (tiempo < 42) zona = 1;
        else zona = 0;

        printf("\n[%02d min] Zona %s\n",
               tiempo,
               zona ? "LUMINOSA" : "OSCURA");

        // --- Ejecutar procesos ---
        int temp = P1(zona);
        int cooling_state = P2(temp);

        printf("[P1] Temp leída: %d C\n", temp);
        printf("[P2] Cooling: %s\n",
               cooling_state ? "ON" : "OFF");

        P3(temp, cooling_state, zona);

        tiempo += 5;
    }

    printf("\n===== FIN DEL ESCENARIO 1 =====\n");
    return 0;
}