#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define ORBIT_TIME_MIN   100
#define BRIGHT_ZONE_MIN   42
#define STEP_MIN           5
#define SLEEP_SECONDS      1

#define T_MIN_DARK   45
#define T_MAX_DARK   75
#define T_MIN_BRIGHT 70
#define T_MAX_BRIGHT 105

#define COOL_ON_TH   90
#define COOL_OFF_TH  60
#define ANOMALY_TH  100

typedef enum { PID_P1 = 0, PID_P3 = 1, PID_P2 = 2 } proc_id_t;
static const proc_id_t PRIORITY_ORDER[3] = { PID_P1, PID_P3, PID_P2 };

typedef struct {
    proc_id_t pid;
    const char *name;
    int pc;
    int active;
    int dirty;
    int lost_bytes;
} pcb_t;

static int minute_sim = 0;
static int temperature = 0;
static int cooling_on = 0;

#define UART_BUF_CAP 64
static char uart_buf[UART_BUF_CAP];
static int uart_len = 0;
static int uart_tx_in_progress = 0;

static proc_id_t current = PID_P1;
static int context_switches = 0;
static int abrupt_switches = 0;

static int in_bright_zone(void) { return minute_sim < BRIGHT_ZONE_MIN; }
static int rand_range(int lo, int hi) { return lo + (rand() % (hi - lo + 1)); }

static int are_consecutive(proc_id_t a, proc_id_t b) {
    for (int i = 0; i < 3; ++i) {
        if (PRIORITY_ORDER[i] == a) {
            if (i > 0 && PRIORITY_ORDER[i - 1] == b) return 1;
            if (i < 2 && PRIORITY_ORDER[i + 1] == b) return 1;
        }
    }
    return 0;
}

/* P1: lectura de temperatura */
static void P1_step(pcb_t *p) {
    p->pc++;
    if (in_bright_zone()) temperature = rand_range(T_MIN_BRIGHT, T_MAX_BRIGHT);
    else temperature = rand_range(T_MIN_DARK, T_MAX_DARK);
    p->dirty = 1;
    printf("[P1] t=%3d min | Temp=%d C | Zona=%s | pc=%d\n",
           minute_sim, temperature, in_bright_zone() ? "Luminosa" : "Oscura", p->pc);
}

/* P2: control de enfriamiento */
static void P2_step(pcb_t *p) {
    p->pc++;
    int prev = cooling_on;
    if (!cooling_on && temperature > COOL_ON_TH) {
        cooling_on = 1;
        printf("[P2] ⚠️ ACTIVADO (T>%d C)\n", COOL_ON_TH);
    } else if (cooling_on && temperature < COOL_OFF_TH) {
        cooling_on = 0;
        printf("[P2] ✅ DESACTIVADO (T<%d C)\n", COOL_OFF_TH);
    } else {
        printf("[P2] Estado=%s | pc=%d\n", cooling_on ? "ON" : "OFF", p->pc);
    }
    p->dirty = cooling_on ? 1 : 0;
    p->active = (cooling_on || prev != cooling_on);
}

/* P3: transmisión UART */
static void P3_step(pcb_t *p) {
    p->pc++;
    if (uart_len == 0) {
        int n = snprintf(uart_buf, UART_BUF_CAP,
                         "UART: T=%dC, COOL=%s\n", temperature, cooling_on ? "ON" : "OFF");
        if (n >= UART_BUF_CAP) n = UART_BUF_CAP - 1;
        uart_len = n;
        uart_tx_in_progress = 1;
        p->dirty = 1;
    }
    int chunk = uart_len > 16 ? 16 : uart_len;
    uart_len -= chunk;
    printf("[P3] TX %2dB (pendiente=%2dB) | pc=%d\n", chunk, uart_len, p->pc);
    if (uart_len == 0) {
        uart_tx_in_progress = 0;
        p->dirty = 0;
    }
}

/* Selección según prioridad o evento */
static proc_id_t choose_next(proc_id_t last, int anomaly) {
    if (anomaly) return PID_P2;
    int idx = 0;
    for (int i = 0; i < 3; ++i) if (PRIORITY_ORDER[i] == last) idx = i;
    return PRIORITY_ORDER[(idx + 1) % 3];
}

/* Cambio de contexto */
static void context_switch(pcb_t *from, pcb_t *to, int abrupt) {
    if (from->pid == to->pid) return;
    context_switches++;

    int non_consecutive = !are_consecutive(from->pid, to->pid);
    if (abrupt && non_consecutive) {
        abrupt_switches++;
        int lost = 0;
        if (from->dirty) {
            if (from->pid == PID_P3 && uart_tx_in_progress) lost = uart_len > 0 ? uart_len : 8;
            else if (from->pid == PID_P1) lost = 4;
            else if (from->pid == PID_P2) lost = 2;
            from->lost_bytes += lost;
        }
        printf("🔁 Cambio ABRUPTO %s -> %s | pérdida=%dB\n",
               from->name, to->name, lost);
    } else {
        printf("↔️  Cambio de contexto %s -> %s\n", from->name, to->name);
    }
}

/* Main */
int main(void) {
    srand(time(NULL));
    pcb_t p1 = { PID_P1, "P1", 0, 1, 0, 0 };
    pcb_t p3 = { PID_P3, "P3", 0, 1, 0, 0 };
    pcb_t p2 = { PID_P2, "P2", 0, 0, 0, 0 };
    pcb_t *procs[3] = { &p1, &p3, &p2 };
    current = PID_P1;

    printf("=== ESCENARIO 2: Prioridad impuesta (P1 > P3 > P2) ===\n");

    for (minute_sim = 0; minute_sim <= ORBIT_TIME_MIN; minute_sim += STEP_MIN) {
        printf("\n⏱️  t=%d min | Zona=%s\n", minute_sim,
               in_bright_zone() ? "Luminosa" : "Oscura");

        pcb_t *cur = procs[current];
        switch (current) {
            case PID_P1: P1_step(cur); break;
            case PID_P2: P2_step(cur); break;
            case PID_P3: P3_step(cur); break;
        }

        int anomaly = (temperature >= ANOMALY_TH);
        proc_id_t next = choose_next(current, anomaly);
        context_switch(cur, procs[next], anomaly);
        current = next;

        printf("[OS] UART=%dB pend | Cooling=%s\n",
               uart_len, cooling_on ? "ON" : "OFF");
        sleep(SLEEP_SECONDS);
    }

    printf("\n===== RESUMEN =====\n");
    printf("Context switches: %d | Abruptos: %d\n", context_switches, abrupt_switches);
    printf("Pérdidas (B): P1=%d, P3=%d, P2=%d\n",
           p1.lost_bytes, p3.lost_bytes, p2.lost_bytes);
    return 0;
}
