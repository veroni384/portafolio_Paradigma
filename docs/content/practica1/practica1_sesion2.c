#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USER 32
#define MAX_DOC 48

typedef struct {
    int id;
    char usuario[MAX_USER];
    char documento[MAX_DOC];
    int paginas_total;
} PrintJob_t;

typedef struct Node_t {
    PrintJob_t job;
    struct Node_t* next;
} Node_t;

typedef struct {
    Node_t* head; // frente
    Node_t* tail; // final
    int size;
} QueueDynamic_t;

// Inicializar cola
void qd_init(QueueDynamic_t* q) {
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
}

// Validar si está vacía
int qd_is_empty(const QueueDynamic_t* q) {
    return q->size == 0;
}

// Agregar trabajo (enqueue)
int qd_enqueue(QueueDynamic_t* q, PrintJob_t job) {
    Node_t* nuevo = (Node_t*) malloc(sizeof(Node_t));
    if (nuevo == NULL) return 0; // malloc falló
    nuevo->job = job;
    nuevo->next = NULL;

    if (qd_is_empty(q)) {
        q->head = nuevo;
        q->tail = nuevo;
    } else {
        q->tail->next = nuevo;
        q->tail = nuevo;
    }
    q->size++;
    return 1;
}

// Ver siguiente trabajo (peek)
int qd_peek(const QueueDynamic_t* q, PrintJob_t* out) {
    if (qd_is_empty(q)) return 0;
    *out = q->head->job;
    return 1;
}

// Procesar trabajo (dequeue)
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

// Liberar toda la memoria
void qd_destroy(QueueDynamic_t* q) {
    Node_t* actual = q->head;
    while (actual != NULL) {
        Node_t* siguiente = actual->next;
        free(actual);
        actual = siguiente;
    }
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
}

// Listar cola
void qd_print(const QueueDynamic_t* q) {
    if (qd_is_empty(q)) {
        printf("Cola vacía\n");
        return;
    }
    Node_t* actual = q->head;
    while (actual != NULL) {
        printf("[id=%d usuario=%s doc=%s pags=%d]\n",
               actual->job.id,
               actual->job.usuario,
               actual->job.documento,
               actual->job.paginas_total);
        actual = actual->next;
    }
}

int main() {
    QueueDynamic_t cola;
    qd_init(&cola);

    int opcion;
    int contador_id = 1;
    PrintJob_t job, temp;

    do {
        printf("\n--- MENU ---\n");
        printf("1. Agregar trabajo\n");
        printf("2. Ver siguiente trabajo\n");
        printf("3. Procesar trabajo\n");
        printf("4. Listar cola\n");
        printf("5. Salir\n");
        printf("Selecciona una opcion: ");
        scanf("%d", &opcion);
        getchar(); // limpiar salto de línea

        switch(opcion) {
            case 1:
                printf("Usuario: ");
                fgets(job.usuario, MAX_USER, stdin);
                job.usuario[strcspn(job.usuario, "\n")] = 0;

                printf("Documento: ");
                fgets(job.documento, MAX_DOC, stdin);
                job.documento[strcspn(job.documento, "\n")] = 0;

                printf("Paginas: ");
                scanf("%d", &job.paginas_total);
                getchar();

                if (job.paginas_total <= 0 || strlen(job.usuario) == 0 || strlen(job.documento) == 0) {
                    printf("Entrada inválida. Verifica los datos.\n");
                    break;
                }

                job.id = contador_id++;
                if (qd_enqueue(&cola, job)) {
                    printf("Agregado -> id=%d usuario=%s doc=%s pags=%d\n",
                           job.id, job.usuario, job.documento, job.paginas_total);
                } else {
                    printf("Error: no se pudo reservar memoria.\n");
                }
                break;

            case 2:
                if (qd_peek(&cola, &temp)) {
                    printf("Siguiente -> id=%d %s %s %d\n",
                           temp.id, temp.usuario, temp.documento, temp.paginas_total);
                } else {
                    printf("Cola vacía\n");
                }
                break;

            case 3:
                if (qd_dequeue(&cola, &temp)) {
                    printf("Procesado -> id=%d %s %s %d\n",
                           temp.id, temp.usuario, temp.documento, temp.paginas_total);
                } else {
                    printf("Cola vacía\n");
                }
                break;

            case 4:
                qd_print(&cola);
                break;

            case 5:
                printf("Saliendo...\n");
                qd_destroy(&cola); // liberar memoria antes de salir
                break;

            default:
                printf("Opcion invalida\n");
        }

    } while(opcion != 5);

    return 0;
}
