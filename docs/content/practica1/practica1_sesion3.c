#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Definiciones y Estructuras ---
#define MAX_USER 32
#define MAX_DOC 48

typedef enum { NORMAL = 0, URGENTE = 1 } Prioridad_t;
typedef enum { EN_COLA = 0, IMPRIMIENDO = 1, COMPLETADO = 2, CANCELADO = 3 } Estado_t;

typedef struct {
    int id;
    char usuario[MAX_USER];
    char documento[MAX_DOC];
    int paginas_total;
    int paginas_restantes;
    Prioridad_t prioridad;
    Estado_t estado;
} PrintJob_t;

typedef struct Node_t {
    PrintJob_t job;
    struct Node_t* next;
} Node_t;

typedef struct {
    Node_t* head;
    Node_t* tail;
    int size;
    // Mejora 3: Estadísticas
    int total_impresos;
    int total_paginas_acumuladas;
} QueueDynamic_t;

// --- Funciones de Gestión de Cola ---
void qd_init(QueueDynamic_t* q) {
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    q->total_impresos = 0;
    q->total_paginas_acumuladas = 0;
}

int qd_is_empty(const QueueDynamic_t* q) { return q->size == 0; }

// Mejora 1: Prioridad (Insertar URGENTE al principio o después de otros URGENTE)
int qd_enqueue_prioridad(QueueDynamic_t* q, PrintJob_t job) {
    Node_t* nuevo = (Node_t*)malloc(sizeof(Node_t));
    if (!nuevo) return 0;
    nuevo->job = job;
    nuevo->next = NULL;

    if (qd_is_empty(q)) {
        q->head = q->tail = nuevo;
    } else if (job.prioridad == URGENTE) {
        // Lógica simple: Los urgentes van al frente
        nuevo->next = q->head;
        q->head = nuevo;
    } else {
        q->tail->next = nuevo;
        q->tail = nuevo;
    }
    q->size++;
    return 1;
}

int qd_dequeue(QueueDynamic_t* q, PrintJob_t* out) {
    if (qd_is_empty(q)) return 0;
    Node_t* temp = q->head;
    *out = temp->job;
    q->head = q->head->next;
    free(temp);
    q->size--;
    if (q->size == 0) q->tail = NULL;
    return 1;
}

void qd_destroy(QueueDynamic_t* q) {
    Node_t* actual = q->head;
    while (actual) {
        Node_t* sig = actual->next;
        free(actual);
        actual = sig;
    }
    q->head = q->tail = NULL;
    q->size = 0;
}

// --- Simulación con Visualización ---


void simular_impresion_raylib(QueueDynamic_t* q) {
    InitWindow(850, 500, "Simulador de Impresión");
    SetTargetFPS(60);

    float timer = 0.0f;
    const float delay = 0.8f; // Segundos por página

    while (!WindowShouldClose()) {
        timer += GetFrameTime();

        // Lógica de simulación
        if (!qd_is_empty(q) && timer >= delay) {
            timer = 0.0f; // Reiniciar timer
            Node_t* actual = q->head;
            
            if (actual->job.estado == EN_COLA) actual->job.estado = IMPRIMIENDO;

            if (actual->job.estado == IMPRIMIENDO) {
                actual->job.paginas_restantes--;
                if (actual->job.paginas_restantes <= 0) {
                    actual->job.estado = COMPLETADO;
                    // Registrar estadísticas antes de eliminar
                    q->total_impresos++;
                    q->total_paginas_acumuladas += actual->job.paginas_total;
                    
                    PrintJob_t temp;
                    qd_dequeue(q, &temp);
                }
            }
        }

        BeginDrawing();
        ClearBackground(GetColor(0x181818FF)); // Fondo oscuro elegante

        DrawText("COLA DE IMPRESIÓN ACTIVA", 50, 20, 20, LIGHTGRAY);
        
        // Dibujar Estadísticas (Mejora 3)
        DrawRectangle(600, 50, 220, 100, DARKGRAY);
        DrawText("ESTADÍSTICAS", 610, 60, 15, YELLOW);
        DrawText(TextFormat("Completados: %d", q->total_impresos), 610, 85, 15, WHITE);
        DrawText(TextFormat("Págs Totales: %d", q->total_paginas_acumuladas), 610, 110, 15, WHITE);

        // Visualización de la cola
        int y_pos = 60;
        Node_t* it = q->head;
        while (it) {
            Color colorEstado = (it->job.prioridad == URGENTE) ? RED : SKYBLUE;
            if (it == q->head) colorEstado = GREEN; // Resaltar el que se está imprimiendo

            DrawRectangleLines(50, y_pos, 520, 60, colorEstado);
            
            // Texto del trabajo
            DrawText(TextFormat("#%d | %s | %s", it->job.id, it->job.usuario, it->job.documento), 65, y_pos + 10, 18, RAYWHITE);
            
            // Barra de progreso (Mejora visual)
            float progreso = (float)(it->job.paginas_total - it->job.paginas_restantes) / it->job.paginas_total;
            DrawRectangle(65, y_pos + 35, 490, 10, DARKGRAY);
            DrawRectangle(65, y_pos + 35, (int)(490 * progreso), 10, colorEstado);

            DrawText((it->job.prioridad == URGENTE ? "URGENTE" : "NORMAL"), 480, y_pos + 10, 12, colorEstado);

            y_pos += 75;
            it = it->next;
        }

        if(qd_is_empty(q)) DrawText("SIN TRABAJOS PENDIENTES", 180, 250, 20, GRAY);

        EndDrawing();
    }
    CloseWindow();
}

int main() {
    QueueDynamic_t cola;
    qd_init(&cola);

    // Datos de prueba para demostrar prioridad y simulación
    PrintJob_t j1 = {101, "Ana", "tesis_final.pdf", 8, 8, NORMAL, EN_COLA};
    PrintJob_t j2 = {102, "Luis", "codigo_lab.c", 3, 3, URGENTE, EN_COLA};
    PrintJob_t j3 = {103, "Prof_Gallegos", "examen.pdf", 5, 5, URGENTE, EN_COLA};

    qd_enqueue_prioridad(&cola, j1);
    qd_enqueue_prioridad(&cola, j2); // Entrará antes que Ana por ser Urgente
    qd_enqueue_prioridad(&cola, j3); // Entrará al frente también

    simular_impresion_raylib(&cola);
    qd_destroy(&cola);

    return 0;
}
