# Reporte de Práctica 01: Cola de Impresión en C

**Materia:** 40032 - Paradigmas de la Programación  
**Docente:** M.I. José Carlos Gallegos Mariscal  
**Grupo:** 941  
**Alumno:** [Tu Nombre Completo]  
**Matrícula:** [Tu Matrícula]  

---

## 1. Introducción
En el ámbito de la computación, la gestión de recursos compartidos es un reto fundamental. Este reporte documenta la resolución del problema de una **cola de impresión** utilizando el lenguaje C. Se implementó una estructura de datos tipo **FIFO (First-In, First-Out)**, la cual es esencial para garantizar que los trabajos de impresión se procesen en el estricto orden en que fueron solicitados, evitando condiciones de carrera y garantizando equidad entre los usuarios.

## 2. Diseño

### Definición de `PrintJob_t`
La estructura principal del sistema agrupa los atributos necesarios para la administración y simulación:
* `id`: Entero autoincremental para control único.
* `usuario[MAX_USER]` y `documento[MAX_DOC]`: Cadenas que identifican el origen del trabajo.
* `paginas_total`: Volumen de hojas del documento original.
* `paginas_restantes`: Atributo dinámico que se decrementa durante la simulación.
* `estado`: Enumerador que define si el trabajo está `EN_COLA`, `IMPRIMIENDO` o `COMPLETADO`.

### Descripción de Modelos
1.  **Cola Estática:** Basada en un arreglo de tamaño fijo con un contador `size`. El frente siempre se ubica en el índice 0, lo que obliga a realizar un *shift* (desplazamiento) de elementos en cada extracción.

2.  **Cola Dinámica:** Implementada mediante una lista enlazada simple con punteros `head` y `tail`. Permite la asignación de memoria en tiempo de ejecución.


## 3. Implementación

### Lista de Funciones
* `qd_init`: Establece la cola en un estado inicial seguro (punteros a NULL).
* `qd_enqueue`: Utiliza `malloc` para crear un nodo y lo enlaza al final de la lista ($O(1)$).
* `qd_dequeue`: Recupera los datos del frente, mueve el `head` y libera la memoria con `free` ($O(1)$).
* `qd_destroy`: Recorre toda la lista liberando cada nodo para evitar *memory leaks*.
* `simular_impresion_raylib`: Gestiona el ciclo de renderizado y la lógica de decremento de páginas.

### Decisiones Relevantes
* **Validación de Entradas:** Se optó por `fgets` y `atoi` para evitar los problemas de limpieza de búfer que genera `scanf`.
* **Gestión de Memoria:** Se implementó una limpieza total al salir del programa para asegurar que el sistema devuelva toda la memoria utilizada al SO.

## 4. Demostración de Conceptos

### Alcance y Duración (Scope & Lifetime)
1.  **Local:** La variable `PrintJob_t aux` dentro de `qd_dequeue` tiene una duración automática; nace y muere con la ejecución de la función.
2.  **Global/Estática:** El contador de IDs en el `main` persiste durante toda la ejecución del programa, permitiendo que cada trabajo tenga un folio único.
3.  **Heap (Dinámica):** Los nodos creados en `qd_enqueue` tienen una duración manual; persisten hasta que se llama explícitamente a `free()`.

### Memoria
* **Stack:** Almacena los punteros de control de la cola y variables locales.
* **Heap:** Almacena la estructura de datos real. Es donde se aloja cada `Node_t` mediante `malloc`.

### Subprogramas y Contratos
* **Punteros:** Las funciones reciben `QueueDynamic_t* q` para poder modificar los punteros `head` y `tail` de la estructura original (paso por referencia).
* **Const:** Se utiliza `const QueueDynamic_t* q` en funciones de lectura para garantizar que el subprograma no altere accidentalmente los datos de la cola.

## 5. Simulación
La simulación en **Raylib** visualiza el progreso mediante una barra que se llena proporcionalmente al trabajo realizado:
$$\text{Progreso} = \frac{\text{págs\_totales} - \text{págs\_restantes}}{\text{págs\_totales}}$$
El retardo (delay) se maneja con `GetFrameTime()`, acumulando tiempo hasta alcanzar 1.0s para procesar una página, manteniendo la fluidez de la ventana.
![Evidencia de la Simulación en Raylib](./captura1.png)

## 6. Análisis Comparativo: Memoria Estática vs. Dinámica

### Limitaciones y Riesgos
La **cola estática** presenta una rigidez crítica: si se define un máximo de 10 trabajos, el sistema fallará ante el trabajo 11, independientemente de si la computadora tiene memoria libre. Por el contrario, la **cola dinámica** es flexible, pero introduce el riesgo de "Memory Leaks" si el programador olvida liberar los nodos.

### Complejidad y Rendimiento
En la implementación estática sugerida, el `dequeue` tiene una complejidad de **$O(n)$** debido a que, tras eliminar el frente, todos los elementos restantes deben desplazarse una posición a la izquierda. En la implementación dinámica, tanto `enqueue` como `dequeue` son **$O(1)$**, ya que solo se requiere manipular punteros, lo que resulta en un rendimiento significativamente superior conforme aumenta el volumen de datos.

### Impacto en el Diseño
La memoria dinámica permite un diseño más limpio basado en contratos de funciones, donde la estructura puede crecer o disminuir según la demanda real del usuario, optimizando el uso de los recursos del sistema operativo.

## 7. Conclusiones
La práctica permitió comprender que la elección de una estructura de datos no solo depende de la facilidad de programación, sino del rendimiento esperado. La migración a memoria dinámica fue fundamental para entender el ciclo de vida de los datos en el *heap* y la importancia de la gestión manual de recursos en C.

## 8. Referencias
* Kernighan, B. W., & Ritchie, D. M. (1988). *The C Programming Language*. Prentice Hall.
* Raylib Technologies. (2024). *Raylib Cheat Sheet*. Recuperado de https://www.raylib.com/cheatsheet/cheatsheet.html