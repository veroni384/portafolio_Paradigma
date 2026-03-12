/*
 * ============================================================
 *  Practica 01 - Sesion 2: Cola de impresion con memoria dinamica
 *  Materia: Paradigmas de la Programacion
 *  Compilar: gcc -std=c11 -Wall -Wextra -Wpedantic -O0 -g sesion2.c -o sesion2
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>   /* malloc, free */
#include <string.h>

/* ─────────────────────────────────────
   CONSTANTES
───────────────────────────────────── */
#define MAX_USER  32
#define MAX_DOC   48

/* ─────────────────────────────────────
   ENUMS
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
    int         paginas_restantes;
    int         copias;
    Prioridad_t prioridad;
    Estado_t    estado;
    int         ms_por_pagina;
} PrintJob_t;

/* ─────────────────────────────────────
   NODO DE LA LISTA ENLAZADA
   Cada nodo guarda un trabajo y un
   puntero al siguiente nodo.
   Estos nodos viven en el HEAP (malloc).
───────────────────────────────────── */
typedef struct Node_t {
    PrintJob_t    job;
    struct Node_t *next;  /* puntero al siguiente nodo */
} Node_t;

/* ─────────────────────────────────────
   ESTRUCTURA DE LA COLA DINAMICA
   head -> frente (el que sale primero)
   tail -> final  (donde se agrega)
───────────────────────────────────── */
typedef struct {
    Node_t *head;   /* frente de la cola */
    Node_t *tail;   /* final de la cola  */
    int     size;
} QueueDynamic_t;

/* ─────────────────────────────────────
   PROTOTIPOS
───────────────────────────────────── */
void qd_init     (QueueDynamic_t *q);
int  qd_is_empty (const QueueDynamic_t *q);
int  qd_enqueue  (QueueDynamic_t *q, PrintJob_t job);
int  qd_peek     (const QueueDynamic_t *q, PrintJob_t *out);
int  qd_dequeue  (QueueDynamic_t *q, PrintJob_t *out);
void qd_destroy  (QueueDynamic_t *q);
void qd_print    (const QueueDynamic_t *q);

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
   FUNCIONES DE LA COLA DINAMICA
   ============================================================ */

/* Inicializa la cola: sin nodos, head y tail apuntan a NULL */
void qd_init(QueueDynamic_t *q)
{
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
}

/* Retorna 1 si esta vacia, 0 si no */
int qd_is_empty(const QueueDynamic_t *q)
{
    return q->size == 0;
}

/*
 * Agrega un trabajo al final de la cola  O(1)
 * Aqui es donde usamos malloc para pedir memoria en el HEAP.
 * Si malloc falla (retorna NULL), no agregamos y retornamos 0.
 *
 * Diferencia con sesion 1:
 * - En sesion 1 el arreglo ya existia en el stack, solo llenabamos posiciones.
 * - Aqui cada nodo es memoria nueva que pedimos al sistema operativo.
 */
int qd_enqueue(QueueDynamic_t *q, PrintJob_t job)
{
    /* Pedimos memoria para un nuevo nodo */
    Node_t *nuevo = (Node_t *)malloc(sizeof(Node_t));

    /* Siempre validar que malloc no haya fallado */
    if (nuevo == NULL) {
        printf("\n  ERROR: No se pudo reservar memoria (malloc fallo).\n");
        return 0;
    }

    nuevo->job  = job;
    nuevo->next = NULL;  /* el nuevo nodo sera el ultimo, no apunta a nadie */

    if (qd_is_empty(q)) {
        /* Cola vacia: head y tail apuntan al mismo nodo */
        q->head = nuevo;
        q->tail = nuevo;
    } else {
        /* Enlazamos el nodo actual final con el nuevo */
        q->tail->next = nuevo;
        q->tail       = nuevo;
    }

    q->size++;
    return 1;
}

/*
 * Consulta el frente SIN modificar la cola  O(1)
 * Usa const porque prometemos no modificar la cola.
 */
int qd_peek(const QueueDynamic_t *q, PrintJob_t *out)
{
    if (qd_is_empty(q)) {
        return 0;
    }
    *out = q->head->job;
    return 1;
}

/*
 * Saca el frente de la cola y libera el nodo  O(1)
 * Aqui usamos free para devolver la memoria al sistema.
 *
 * Diferencia con sesion 1:
 * - En sesion 1 teniamos que hacer shift (mover todos los elementos).
 * - Aqui solo movemos head al siguiente nodo y liberamos el anterior.
 * - Por eso es O(1) en lugar de O(n).
 */
int qd_dequeue(QueueDynamic_t *q, PrintJob_t *out)
{
    if (qd_is_empty(q)) {
        return 0;
    }

    Node_t *nodo_a_liberar = q->head;
    *out   = nodo_a_liberar->job;
    q->head = nodo_a_liberar->next;  /* avanzamos el frente */

    /* Si la cola quedo vacia, tail tambien debe ser NULL */
    if (q->head == NULL) {
        q->tail = NULL;
    }

    free(nodo_a_liberar);   /* devolvemos la memoria al sistema */
    q->size--;
    return 1;
}

/*
 * Libera TODOS los nodos de la cola.
 * Obligatorio llamar esto al salir del programa.
 * Si no se llama: fuga de memoria (los nodos quedan en el heap sin dueno).
 */
void qd_destroy(QueueDynamic_t *q)
{
    Node_t *actual = q->head;
    while (actual != NULL) {
        Node_t *siguiente = actual->next;
        free(actual);
        actual = siguiente;
    }
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
}

/* Imprime todos los trabajos en orden de atencion */
void qd_print(const QueueDynamic_t *q)
{
    if (qd_is_empty(q)) {
        printf("\n  La cola esta vacia.\n");
        return;
    }

    printf("\n  === Cola de impresion (%d trabajos) ===\n", q->size);
    Node_t *actual = q->head;
    int     i      = 1;

    while (actual != NULL) {
        PrintJob_t *j = &actual->job;
        printf("\n  Trabajo #%d\n", i++);
        printf("  ID       : %d\n",  j->id);
        printf("  Usuario  : %s\n",  j->usuario);
        printf("  Documento: %s\n",  j->documento);
        printf("  Paginas  : %d\n",  j->paginas_total);
        printf("  Copias   : %d\n",  j->copias);
        printf("  Prioridad: %s\n",  j->prioridad == URGENTE ? "URGENTE" : "NORMAL");
        printf("  Estado   : %s\n",  nombre_estado(j->estado));
        printf("  ---------------------------\n");
        actual = actual->next;
    }
}

/* ============================================================
   FUNCIONES DE APOYO
   ============================================================ */

void limpiar_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

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

void pedir_trabajo(PrintJob_t *pJob, int id_actual)
{
    char buffer[128];

    pJob->id = id_actual;

    do {
        printf("  Usuario (no puede estar vacio): ");
        fgets(pJob->usuario, MAX_USER, stdin);
        pJob->usuario[strcspn(pJob->usuario, "\n")] = '\0';
    } while (pJob->usuario[0] == '\0');

    do {
        printf("  Documento (no puede estar vacio): ");
        fgets(pJob->documento, MAX_DOC, stdin);
        pJob->documento[strcspn(pJob->documento, "\n")] = '\0';
    } while (pJob->documento[0] == '\0');

    do {
        printf("  Total de paginas (debe ser > 0): ");
        fgets(buffer, sizeof(buffer), stdin);
        pJob->paginas_total = (int)strtol(buffer, NULL, 10);
    } while (pJob->paginas_total <= 0);

    do {
        printf("  Copias (debe ser >= 1): ");
        fgets(buffer, sizeof(buffer), stdin);
        pJob->copias = (int)strtol(buffer, NULL, 10);
    } while (pJob->copias < 1);

    int p = -1;
    do {
        printf("  Prioridad (0=NORMAL, 1=URGENTE): ");
        fgets(buffer, sizeof(buffer), stdin);
        p = (int)strtol(buffer, NULL, 10);
    } while (p != 0 && p != 1);
    pJob->prioridad = (Prioridad_t)p;

    pJob->paginas_restantes = pJob->paginas_total;
    pJob->estado            = EN_COLA;
    pJob->ms_por_pagina     = 300;
}

int mostrar_menu(void)
{
    char buffer[16];
    int  op;

    system("cls");
    printf("\n  ================================\n");
    printf("    COLA DE IMPRESION - Sesion 2  \n");
    printf("    (Memoria Dinamica)             \n");
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

void menu(void)
{
    QueueDynamic_t cola;
    qd_init(&cola);

    int        op;
    int        id_contador = 1;
    PrintJob_t trabajo;

    do {
        op = mostrar_menu();

        switch (op) {

        case 1:
            printf("\n  --- Nuevo trabajo ---\n");
            pedir_trabajo(&trabajo, id_contador);
            if (qd_enqueue(&cola, trabajo)) {
                id_contador++;
                printf("\n  Trabajo agregado. ID asignado: %d\n", trabajo.id);
            }
            printf("\n  Presiona Enter para continuar...");
            limpiar_buffer();
            break;

        case 2:
            if (qd_peek(&cola, &trabajo)) {
                printf("\n  --- Siguiente en cola ---\n");
                printf("  ID       : %d\n",  trabajo.id);
                printf("  Usuario  : %s\n",  trabajo.usuario);
                printf("  Documento: %s\n",  trabajo.documento);
                printf("  Paginas  : %d\n",  trabajo.paginas_total);
                printf("  Estado   : %s\n",  nombre_estado(trabajo.estado));
            } else {
                printf("\n  ERROR: La cola esta vacia.\n");
            }
            printf("\n  Presiona Enter para continuar...");
            limpiar_buffer();
            break;

        case 3:
            if (qd_dequeue(&cola, &trabajo)) {
                printf("\n  --- Procesando trabajo ---\n");
                printf("  ID       : %d\n",  trabajo.id);
                printf("  Usuario  : %s\n",  trabajo.usuario);
                printf("  Documento: %s\n",  trabajo.documento);
                printf("  Paginas  : %d\n",  trabajo.paginas_total);
                printf("  Nodo liberado de la memoria.\n");
            } else {
                printf("\n  ERROR: La cola esta vacia.\n");
            }
            printf("\n  Presiona Enter para continuar...");
            limpiar_buffer();
            break;

        case 4:
            qd_print(&cola);
            printf("\n  Presiona Enter para continuar...");
            limpiar_buffer();
            break;

        case 5:
            printf("\n  Liberando memoria antes de salir...\n");
            qd_destroy(&cola);   /* OBLIGATORIO: evita fuga de memoria */
            printf("  Memoria liberada. Hasta luego.\n");
            break;

        default:
            printf("\n  Opcion invalida.\n");
            printf("\n  Presiona Enter para continuar...");
            limpiar_buffer();
        }

    } while (op != 5);
}

