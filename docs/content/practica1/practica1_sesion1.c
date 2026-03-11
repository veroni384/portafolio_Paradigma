#include <stdio.h>
#include <string.h>

#define MAX_USER 32
#define MAX_DOC 48
#define MAX_JOBS 10

typedef struct {
    int id;
    char usuario[MAX_USER];
    char documento[MAX_DOC];
    int paginas_total;
} PrintJob_t;

typedef struct {
    PrintJob_t data[MAX_JOBS];
    int size;
} QueueStatic_t;

// Inicializar cola
void qs_init(QueueStatic_t* q) {
    q->size = 0;
}

// Validar si está vacía
int qs_is_empty(const QueueStatic_t* q) {
    return q->size == 0;
}

// Validar si está llena
int qs_is_full(const QueueStatic_t* q) {
    return q->size == MAX_JOBS;
}

// Agregar trabajo (enqueue)
int qs_enqueue(QueueStatic_t* q, PrintJob_t job) {
    if (qs_is_full(q)) return 0;
    q->data[q->size] = job;
    q->size++;
    return 1;
}

// Ver siguiente trabajo (peek)
int qs_peek(const QueueStatic_t* q, PrintJob_t* out) {
    if (qs_is_empty(q)) return 0;
    *out = q->data[0];
    return 1;
}

// Procesar trabajo (dequeue)
int qs_dequeue(QueueStatic_t* q, PrintJob_t* out) {
    if (qs_is_empty(q)) return 0;
    *out = q->data[0];
    for (int i = 1; i < q->size; i++) {
        q->data[i - 1] = q->data[i]; // shift
    }
    q->size--;
    return 1;
}

// Listar cola
void qs_print(const QueueStatic_t* q) {
    if (qs_is_empty(q)) {
        printf("Cola vacía\n");
        return;
    }
    for (int i = 0; i < q->size; i++) {
        printf("[id=%d usuario=%s doc=%s pags=%d]\n",
               q->data[i].id,
               q->data[i].usuario,
               q->data[i].documento,
               q->data[i].paginas_total);
    }
}

int main() {
    QueueStatic_t cola;
    qs_init(&cola);

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
                if (qs_is_full(&cola)) {
                    printf("Cola llena, no se puede agregar.\n");
                    break;
                }
                printf("Usuario: ");
                fgets(job.usuario, MAX_USER, stdin);
                job.usuario[strcspn(job.usuario, "\n")] = 0;

                printf("Documento: ");
                fgets(job.documento, MAX_DOC, stdin);
                job.documento[strcspn(job.documento, "\n")] = 0;

                printf("Paginas: ");
                scanf("%d", &job.paginas_total);
                getchar();

                job.id = contador_id++;
                if (qs_enqueue(&cola, job)) {
                    printf("Agregado -> id=%d usuario=%s doc=%s pags=%d\n",
                           job.id, job.usuario, job.documento, job.paginas_total);
                }
                break;

            case 2:
                if (qs_peek(&cola, &temp)) {
                    printf("Siguiente -> id=%d %s %s %d\n",
                           temp.id, temp.usuario, temp.documento, temp.paginas_total);
                } else {
                    printf("Cola vacía\n");
                }
                break;

            case 3:
                if (qs_dequeue(&cola, &temp)) {
                    printf("Procesado -> id=%d %s %s %d\n",
                           temp.id, temp.usuario, temp.documento, temp.paginas_total);
                } else {
                    printf("Cola vacía\n");
                }
                break;

            case 4:
                qs_print(&cola);
                break;

            case 5:
                printf("Saliendo...\n");
                break;

            default:
                printf("Opcion invalida\n");
        }

    } while(opcion != 5);

    return 0;
}

