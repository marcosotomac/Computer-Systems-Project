#include <stdio.h>
#include <stdlib.h>

#define ORBIT_MINUTES 100
#define STEP_MINUTES 5
#define BRIGHT_ZONE_LIMIT 42
#define DATASET_REQUIRED_SAMPLES (ORBIT_MINUTES / STEP_MINUTES)
#define MAX_DATASET_SAMPLES 64
#define DEFAULT_DATASET_PATH "data/dataset_case1.txt"

extern int P1(int zone);
extern int P2(int temp);
extern void P3(int temp, int cooling, int zone);
extern void P1_set_dataset(const int *data, int length);

static int dataset_buffer[MAX_DATASET_SAMPLES];
static int dataset_len = 0;

static void load_dataset_or_exit(const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "Error: no se pudo abrir el dataset '%s'\n", path);
        exit(1);
    }

    dataset_len = 0;
    while (dataset_len < MAX_DATASET_SAMPLES &&
           fscanf(fp, "%d", &dataset_buffer[dataset_len]) == 1) {
        dataset_len++;
    }
    fclose(fp);

    if (dataset_len < DATASET_REQUIRED_SAMPLES) {
        fprintf(stderr,
                "Error: dataset '%s' tiene %d muestras, se requieren %d.\n",
                path, dataset_len, DATASET_REQUIRED_SAMPLES);
        exit(1);
    }

    P1_set_dataset(dataset_buffer, dataset_len);
    printf("Dataset cargado: %s (%d muestras)\n", path, dataset_len);
}

int main(int argc, char **argv) {
    const char *dataset_path = DEFAULT_DATASET_PATH;
    if (argc > 1 && argv[1][0] != '\0') {
        dataset_path = argv[1];
    }

    printf("===== ESCENARIO 1 – BASELINE =====\n");
    load_dataset_or_exit(dataset_path);

    int tiempo = 0;
    int zona = 1;   // 1=luminosa, 0=oscura

    while (tiempo < ORBIT_MINUTES) {

        if (tiempo < BRIGHT_ZONE_LIMIT) zona = 1;
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

        tiempo += STEP_MINUTES;
    }

    printf("\n===== FIN DEL ESCENARIO 1 =====\n");
    return 0;
}
