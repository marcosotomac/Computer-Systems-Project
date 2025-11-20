#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ORBIT_MINUTES 100
#define STEP_MINUTES 5
#define BRIGHT_ZONE_LIMIT 42
#define DATASET_REQUIRED_SAMPLES (ORBIT_MINUTES / STEP_MINUTES)
#define MAX_DATASET_SAMPLES 64
#define DEFAULT_DATASET_PATH "../data/dataset_case1.txt"

extern int P1(int zone);
extern int P2(int temp);
extern void P3(int temp, int cooling, int zone);
extern void P1_set_dataset(const int *data, int length);

enum proc_id { PROC_P1 = 0, PROC_P2, PROC_P3, PROC_COUNT };

static int dataset_buffer[MAX_DATASET_SAMPLES];
static int dataset_len = 0;
static struct {
    double total_us[PROC_COUNT];
    long calls[PROC_COUNT];
} metrics = {0};

static double clock_diff_us(clock_t start, clock_t end) {
    return ((double)(end - start) * 1000000.0) / (double)CLOCKS_PER_SEC;
}

static void record_metric(enum proc_id id, double elapsed_ms) {
    if (id < 0 || id >= PROC_COUNT) return;
    metrics.total_us[id] += elapsed_ms;
    metrics.calls[id]++;
}

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

static void print_metrics(clock_t start_clock, clock_t end_clock) {
    double avg_us[PROC_COUNT] = {0};
    double max_avg = 0.0;
    const char *names[PROC_COUNT] = { "P1", "P2", "P3" };
    double active_us = 0.0;

    for (int i = 0; i < PROC_COUNT; ++i) {
        if (metrics.calls[i] > 0) {
            avg_us[i] = metrics.total_us[i] / (double)metrics.calls[i];
            if (avg_us[i] > max_avg) max_avg = avg_us[i];
        }
        active_us += metrics.total_us[i];
    }

    double wall_ms = clock_diff_us(start_clock, end_clock) / 1000.0;
    double cpu_occupation = wall_ms > 0.0 ? ((active_us / 1000.0) / wall_ms) * 100.0 : 0.0;
    size_t mem_bytes = (size_t)dataset_len * sizeof(int) + sizeof(metrics);
    double mem_kb = (double)mem_bytes / 1024.0;

    printf("\n===== METRICAS ESCENARIO 1 =====\n");
    printf("Proceso | Tiempo total (us) | Promedio (us) | Speedup (relativo)\n");
    for (int i = 0; i < PROC_COUNT; ++i) {
        double speedup = (avg_us[i] > 0.0 && max_avg > 0.0) ? (max_avg / avg_us[i]) : 0.0;
        printf("  %-3s   | %16.3f | %13.3f | %6.2f x\n",
               names[i], metrics.total_us[i], avg_us[i], speedup);
    }

    printf("\nTotal runtime: %.3f ms\n", wall_ms);
    printf("CPU Occupation: %.2f %% (sobre tiempo medido en procesos)\n", cpu_occupation);
    printf("Mem. Occupation: %.2f KB (dataset + métricas)\n", mem_kb);
}

int main(int argc, char **argv) {
    const char *dataset_path = DEFAULT_DATASET_PATH;
    if (argc > 1 && argv[1][0] != '\0') {
        dataset_path = argv[1];
    }

    printf("===== ESCENARIO 1 – BASELINE =====\n");
    load_dataset_or_exit(dataset_path);

    clock_t wall_start = clock();

    int tiempo = 0;
    int zona = 1;   // 1=luminosa, 0=oscura

    while (tiempo < ORBIT_MINUTES) {

        if (tiempo < BRIGHT_ZONE_LIMIT) zona = 1;
        else zona = 0;

        printf("\n[%02d min] Zona %s\n",
               tiempo,
               zona ? "LUMINOSA" : "OSCURA");

        // --- Ejecutar procesos ---
        clock_t t0 = clock();
        int temp = P1(zona);
        clock_t t1 = clock();
        record_metric(PROC_P1, clock_diff_us(t0, t1));

        t0 = clock();
        int cooling_state = P2(temp);
        t1 = clock();
        record_metric(PROC_P2, clock_diff_us(t0, t1));

        printf("[P1] Temp leída: %d C\n", temp);
        printf("[P2] Cooling: %s\n",
               cooling_state ? "ON" : "OFF");

        t0 = clock();
        P3(temp, cooling_state, zona);
        t1 = clock();
        record_metric(PROC_P3, clock_diff_us(t0, t1));

        tiempo += STEP_MINUTES;
    }

    printf("\n===== FIN DEL ESCENARIO 1 =====\n");
    clock_t wall_end = clock();
    print_metrics(wall_start, wall_end);
    return 0;
}
