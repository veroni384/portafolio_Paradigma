/*
 * ============================================================
 *  Practica 01 - Sesion 1: Cola de impresion con memoria estatica
 *  Materia: Paradigmas de la Programacion
 *  Compilar: gcc -std=c11 -Wall -Wextra -Wpedantic -O0 -g sesion1.c -o sesion1
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>   /* strtol, system */
#include <string.h>

/* ─────────────────────────────────────
   CONSTANTES
───────────────────────────────────── */
#define MAX_JOBS  10
#define MAX_USER  32
#define MAX_DOC   48

/* ─────────────────────────────────────
   ENUMS
   En C11 se definen con typedef para
   poder usarlos como tipo directamente
───────────────────────────────────── */
typedef enum { NORMAL = 0, URGENTE = 1 } Prioridad_t;

typedef enum {
    EN_COLA      = 0,
    IMPRIMIENDO  = 1,
    COMPLETADO   = 2,
    CANCELADO    = 3
} Estado_t;

/* ─────────────────────────────────────
   ESTRUCTURA DEL TRABAJO
───────────────────────────────────── */
typedef struct {
    int         id;
    char        usuario[MAX_USER];
    char        documento[MAX_DOC];
    int         paginas_total;
    int         paginas_restantes;  /* para simular progreso en sesion 3 */
    int         copias;
    Prioridad_t prioridad;
    Estado_t    estado;
    int         ms_por_pagina;      /* delay por pagina en simulacion     */
} PrintJob_t;

/* ─────────────────────────────────────
   ESTRUCTURA DE LA COLA ESTATICA
   El profe pide diseno simple:
   - data[0] siempre es el frente
   - enqueue agrega en data[size]
   - dequeue hace shift (desplazamiento)
───────────────────────────────────── */
typedef struct {
    PrintJob_t data[MAX_JOBS];
    int        size;   /* cantidad actual de elementos */
} QueueStatic_t;

/* ─────────────────────────────────────
   PROTOTIPOS
───────────────────────────────────── */
void qs_init     (QueueStatic_t *q);
int  qs_is_empty (const QueueStatic_t *q);
int  qs_is_full  (const QueueStatic_t *q);
int  qs_enqueue  (QueueStatic_t *q, PrintJob_t job);
int  qs_peek     (const QueueStatic_t *q, PrintJob_t *out);
int  qs_dequeue  (QueueStatic_t *q, PrintJob_t *out);
void qs_print    (const QueueStatic_t *q);

void       menu          (void);
int        mostrar_menu  (void);
void       pedir_trabajo (PrintJob_t *pJob, int id_actual);
void       limpiar_buffer(void);
const char *nombre_estado(Estado_t e);

/* ─────────────────────────────────────
   MAIN
───────────────────────────────────── */
int main(void)
{
    menu();
    return 0;
}

/* ============================================================
   FUNCIONES DE LA COLA
   ============================================================ */

/* Inicializa la cola, size = 0 */
void qs_init(QueueStatic_t *q)
{
    q->size = 0;
}

/* Retorna 1 si esta vacia, 0 si no */
int qs_is_empty(const QueueStatic_t *q)
{
    return q->size == 0;
}

/* Retorna 1 si esta llena, 0 si no */
int qs_is_full(const QueueStatic_t *q)
{
    return q->size == MAX_JOBS;
}

/*
 * Agrega un trabajo al final de la cola  O(1)
 * Retorna 1 si tuvo exito, 0 si estaba llena
 */
int qs_enqueue(QueueStatic_t *q, PrintJob_t job)
{
    if (qs_is_full(q)) {
        return 0;
    }
    q->data[q->size] = job;
    q->size++;
    return 1;
}

/*
 * Consulta el frente SIN modificar la cola  O(1)
 * Retorna 1 si habia elemento, 0 si estaba vacia
 */
int qs_peek(const QueueStatic_t *q, PrintJob_t *out)
{
    if (qs_is_empty(q)) {
        return 0;
    }
    *out = q->data[0];
    return 1;
}

/*
 * Saca el frente y desplaza el resto (shift)  O(n)
 * Retorna 1 si tuvo exito, 0 si estaba vacia
 *
 * Por que O(n)?
 * Porque despues de sacar data[0], hay que mover
 * data[1]->data[0], data[2]->data[1], etc.
 * Esto es el "costo" de la memoria estatica simple.
 * En sesion 2 (lista enlazada) este costo desaparece.
 */
int qs_dequeue(QueueStatic_t *q, PrintJob_t *out)
{
    if (qs_is_empty(q)) {
        return 0;
    }

    *out = q->data[0];

    /* shift: desplazar todos hacia la izquierda */
    for (int i = 1; i < q->size; i++) {
        q->data[i - 1] = q->data[i];
    }
    q->size--;
    return 1;
}

/* Imprime todos los trabajos en orden de atencion */
void qs_print(const QueueStatic_t *q)
{
    if (qs_is_empty(q)) {
        printf("\n  La cola esta vacia.\n");
        return;
    }

    printf("\n  === Cola de impresion (%d/%d) ===\n", q->size, MAX_JOBS);
    for (int i = 0; i < q->size; i++) {
        const PrintJob_t *j = &q->data[i];
        printf("\n  Trabajo #%d\n", i + 1);
        printf("  ID       : %d\n",   j->id);
        printf("  Usuario  : %s\n",   j->usuario);
        printf("  Documento: %s\n",   j->documento);
        printf("  Paginas  : %d\n",   j->paginas_total);
        printf("  Copias   : %d\n",   j->copias);
        printf("  Prioridad: %s\n",   j->prioridad == URGENTE ? "URGENTE" : "NORMAL");
        printf("  Estado   : %s\n",   nombre_estado(j->estado));
        printf("  ---------------------------\n");
    }
}

/* ============================================================
   FUNCIONES DE APOYO
   ============================================================ */

/* Limpia el buffer del teclado para evitar basura con fgets */
void limpiar_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Convierte el enum Estado_t a texto legible */
const char *nombre_estado(Estado_t e)
{
    switch (e) {
        case EN_COLA:     return "EN_COLA";
        case IMPRIMIENDO: return "IMPRIMIENDO";
        case COMPLETADO:  return "COMPLETADO";
        case CANCELADO:   return "CANCELADO";
        default:          return "DESCONOCIDO";
    }
}

/*
 * Pide los datos de un nuevo trabajo al usuario.
 * Usa fgets para leer strings (mas seguro que scanf).
 * Retorna 1 si los datos son validos, 0 si no.
 */
void pedir_trabajo(PrintJob_t *pJob, int id_actual)
{
    char buffer[128];

    pJob->id = id_actual;

    /* Leer usuario */
    do {
        printf("  Usuario (no puede estar vacio): ");
        fgets(pJob->usuario, MAX_USER, stdin);
        /* Quitar el salto de linea que deja fgets */
        pJob->usuario[strcspn(pJob->usuario, "\n")] = '\0';
    } while (pJob->usuario[0] == '\0');

    /* Leer documento */
    do {
        printf("  Documento (no puede estar vacio): ");
        fgets(pJob->documento, MAX_DOC, stdin);
        pJob->documento[strcspn(pJob->documento, "\n")] = '\0';
    } while (pJob->documento[0] == '\0');

    /* Leer paginas con validacion */
    do {
        printf("  Total de paginas (debe ser > 0): ");
        fgets(buffer, sizeof(buffer), stdin);
        pJob->paginas_total = (int)strtol(buffer, NULL, 10);
    } while (pJob->paginas_total <= 0);

    /* Leer copias con validacion */
    do {
        printf("  Copias (debe ser >= 1): ");
        fgets(buffer, sizeof(buffer), stdin);
        pJob->copias = (int)strtol(buffer, NULL, 10);
    } while (pJob->copias < 1);

    /* Leer prioridad */
    int p = -1;
    do {
        printf("  Prioridad (0=NORMAL, 1=URGENTE): ");
        fgets(buffer, sizeof(buffer), stdin);
        p = (int)strtol(buffer, NULL, 10);
    } while (p != 0 && p != 1);
    pJob->prioridad = (Prioridad_t)p;

    /* Valores fijos para sesion 1 */
    pJob->paginas_restantes = pJob->paginas_total;
    pJob->estado            = EN_COLA;
    pJob->ms_por_pagina     = 300;
}

/* Muestra el menu y retorna la opcion elegida */
int mostrar_menu(void)
{
    char buffer[16];
    int  op;

    system("cls");   /* Windows: cls | Linux/Mac: clear */
    printf("\n  ================================\n");
    printf("    COLA DE IMPRESION - Sesion 1  \n");
    printf("    (Memoria Estatica)             \n");
    printf("  ================================\n");
    printf("  1. Agregar trabajo\n");
    printf("  2. Ver siguiente (Peek)\n");
    printf("  3. Procesar siguiente (Dequeue)\n");
    printf("  4. Listar cola\n");
    printf("  5. Salir\n");
    printf("  ================================\n");
    printf("  Opcion: ");
    fgets(buffer, sizeof(buffer), stdin);
    op = (int)strtol(buffer, NULL, 10);
    return op;
}

/* Bucle principal del menu */
void menu(void)
{
    QueueStatic_t cola;
    qs_init(&cola);

    int        op;
    int        id_contador = 1;
    PrintJob_t trabajo;

    do {
        op = mostrar_menu();

        switch (op) {

        /* ── Agregar ── */
        case 1:
            if (qs_is_full(&cola)) {
                printf("\n  ERROR: La cola esta llena (maximo %d trabajos).\n", MAX_JOBS);
            } else {
                printf("\n  --- Nuevo trabajo ---\n");
                pedir_trabajo(&trabajo, id_contador);
                qs_enqueue(&cola, trabajo);
                id_contador++;
                printf("\n  Trabajo agregado correctamente. ID asignado: %d\n", trabajo.id);
            }
            printf("\n  Presiona Enter para continuar...");
            limpiar_buffer();
            break;

        /* ── Peek ── */
        case 2:
            if (qs_peek(&cola, &trabajo)) {
                printf("\n  --- Siguiente en cola ---\n");
                printf("  ID       : %d\n",  trabajo.id);
                printf("  Usuario  : %s\n",  trabajo.usuario);
                printf("  Documento: %s\n",  trabajo.documento);
                printf("  Paginas  : %d\n",  trabajo.paginas_total);
                printf("  Estado   : %s\n",  nombre_estado(trabajo.estado));
            } else {
                printf("\n  ERROR: La cola esta vacia, no hay nada que ver.\n");
            }
            printf("\n  Presiona Enter para continuar...");
            limpiar_buffer();
            break;

        /* ── Dequeue ── */
        case 3:
            if (qs_dequeue(&cola, &trabajo)) {
                printf("\n  --- Procesando trabajo ---\n");
                printf("  ID       : %d\n",  trabajo.id);
                printf("  Usuario  : %s\n",  trabajo.usuario);
                printf("  Documento: %s\n",  trabajo.documento);
                printf("  Paginas  : %d\n",  trabajo.paginas_total);
                printf("  Trabajo removido de la cola.\n");
            } else {
                printf("\n  ERROR: La cola esta vacia, no hay nada que procesar.\n");
            }
            printf("\n  Presiona Enter para continuar...");
            limpiar_buffer();
            break;

        /* ── Listar ── */
        case 4:
            qs_print(&cola);
            printf("\n  Presiona Enter para continuar...");
            limpiar_buffer();
            break;

        /* ── Salir ── */
        case 5:
            printf("\n  Saliendo...\n");
            break;

        default:
            printf("\n  Opcion invalida, intenta de nuevo.\n");
            printf("\n  Presiona Enter para continuar...");
            limpiar_buffer();
        }

    } while (op != 5);
}

