# Análisis del simulador de memoria

El simulador modela la traducción de direcciones virtuales, la asignacion de memoria física y el reemplazo de páginas cuando no quedan frames libres. Se ejecutaron los tres casos incluidos en `tests/cases/` con una memoria física de 256 KB.

```shell
make test
```

La memoria tiene páginas de 4 KB, por lo que 256 KB contienen 64 frames. El programa cuenta como acceso cada `read` o `write` que llega a la MMU (Management Memory Unit). Un acceso que encuentra una PTE (Page Table Entry) invalida produce un fallo de página. Si ya no hay frames libres, ese fallo produce también un reemplazo.

El $Hit_{rate}$ se calcula como:

$$
Hit_{rate} = ( \text{Accesos totales} - \text{Fallos de página}) / \text{Accesos totales} * 100
$$

Por tanto, un fallo de página se considera un $Miss$ y un acceso con PTE válida se considera un $Hit$.

## 1. Estructuras de datos

### Direccion virtual

`VirtualAddress` representa una direccion virtual de 32 bits. El formato es:

$$
\begin{array}{|c|c|c|}
\hline
\underbrace{0 \quad 1 \quad \dots \quad 9}_{\text{índice PT1: 10 bits}} & 
\underbrace{10 \quad 11 \quad \dots \quad 19}_{\text{índice PT2: 10 bits}} & 
\underbrace{20 \quad 21 \quad \dots \quad 31}_{\text{offset: 12 bits}} \\
\hline
\end{array}
$$

El offset de 12 bits es para el tamaño de pagina en 4 KB. La dirección también puede convertirse en un número de Página Virtual (`VPN`) mediante un desplazamiento de 12 bits.

### Tabla de páginas

`PageTable` implementa una tabla de páginas de dos niveles. El primer nivel contiene hasta 1024 entradas y cada entrada apunta a un arreglo de 1024 entradas del segundo nivel. Cada entrada es una `PTE` con:

- `valid`: indica si la pagina esta actualmente cargada.
- `frame`: identifica el frame fisico asignado.
- `accessed`: registra que la pagina fue accedida.
- `dirty`: registra que la pagina recibio una escritura.

Esto permite mapear páginas virtuales a frames físicos sin crear todos los niveles de la tabla desde el inicio.

### Memoria fisica

`PhysicalMemory` mantiene:

- Un vector de bytes que representa la RAM simulada.
- Una cola de frames libres.
- Un vector `frameOwner` que relaciona cada frame con su VPN actual.

Al asignar un frame por primera vez o después de una expulsión, el MMU lo inicializa a cero. Al reemplazar una página, se invalida su PTE, se libera el frame y se reutiliza para la nueva página.

### Allocator virtual

`VirtualAllocator` usa un allocator con el que las asignaciones avanzan desde la direccion virtual cero y se redondean a paginas completas. `free` marca una asignacion como liberada y permite desmapear sus páginas.

El rango virtual no vuelve a utilizarse para asignaciones futuras lo que simplifica el manejo de fragmentacion y hace determinista la generación de direcciones, aunque limita la reutilización del espacio virtual.

### MMU

`Mmu` coordina la tabla de páginas, la memoria física y la política de reemplazo. En cada traduccion:

1. Incrementa `totalAccesses`.
2. Consulta la PTE.
3. Si la PTE es inválida, atiende un fallo de página.
4. Si la PTE es válida, notifica un acceso a la política.
5. Marca la entrada como accedida y, si aplica, como modificada.

`Stats` almacena accesos totales, fallos, reemplazos y tiempo de procesamiento. El contador de reemplazos solo aumenta cuando el fallo ocurre sin frames libres.

## 2. Politicas de reemplazo

Se implementan dos políticas **FIFO** y **LRU**, ambas implementan la interfaz `IReplacementPolicy`, de modo que el MMU no depende de una política concreta.

### FIFO

FIFO mantiene los frames en el orden en que fueron cargados. El frame más antiguo se coloca al frente de la cola y se selecciona como candidato a reemplazo. Los accesos posteriores no cambian ese orden:

$$
\begin{aligned}
\texttt{onFrameLoaded} &\to \text{agrega el frame al final} \\
\texttt{onAccess}      &\to \text{no modifica el orden} \\
\texttt{pickVictim}    &\to \text{devuelve el frame del frente}
\end{aligned}
$$

La política FIFO es una implementación simple y bajo costo de mantenimiento, pero puede reemplazzar una página muy utilizada o accedida recientemente si fue cargada hace mucho tiempo.

### LRU

LRU mantiene los frames ordenados según su último acceso. Una carga o un acceso mueve el frame al final de la lista. El frame candidato a ser reemplazado es el que esta al frente, es decir, el menos utilizado recientemente:

$$
\begin{aligned}
\texttt{onFrameLoaded} &\to \text{marca el frame como el más reciente} \\
\texttt{onAccess}      &\to \text{mueve el frame al final} \\
\texttt{pickVictim}    &\to \text{devuelve el frame menos reciente}
\end{aligned}
$$

EL LRU aprovecha la localidad temporal de los programas, pero requiere actualizar la estructura en cada acceso, por lo que tiene mayor costo de mantenimiento y computacional que FIFO.

## 3. Resultados de las pruebas

### Resumen

| Caso | Política | Accesos | Fallos | $Hit_{rate}$ | Reemplazos |
|---|---:|---:|---:|---:|---:|
| Test 1: Baseline | FIFO | 10 | 4 | $60.0 \% $ | 0 |
| Test 2: Reemplazo forzado | FIFO | 66 | 66 | $0.0 \% $ | 2 |
| Test 2: Reemplazo forzado | LRU | 66 | 66 | $0.0 \% $ | 2 |
| Test 3: Localidad temporal | FIFO | 69 | 67 | $2.9 \% $ | 3 |
| Test 3: Localidad temporal | LRU | 69 | 65 | $5.8 \% $ | 1 |

### Test 1: Baseline

El programa carga cuatro páginas en una memoria con 64 frames disponibles. Las primeras cuatro escrituras producen cuatro fallos. Las seis lecturas posteriores encuentran las páginas residentes.

- FIFO: 10 accesos, 4 fallos, $60.0 \% $ de $Hit_{rate}$ y 0 reemplazos.

El resultado confirma que una carga de trabajo cuyo conjunto activo cabe en RAM no necesita expulsiones.

### Test 2: Reemplazo forzado

Se cargan 65 páginas en una memoria de 64 frames y luego se vuelve a leer la pagina 0. Cada acceso corresponde a una página nueva o a la página reemplazada, por lo que no hay hits.

- FIFO: 66 accesos, 66 fallos, $0.00 \% $ de $Hit_{rate}$ y 2 reemplazos.
- LRU: 66 accesos, 66 fallos, $0.00 \% $ de $Hit_{rate}$ y 2 reemplazos.

Las políticas producen el mismo resultado porque no existe localidad temporal útil: cuando se vuelve a solicitar la pagina 0, ya fue expulsada, y la secuencia no ofrece accesos repetidos que permitan a LRU distinguir una página accedida recientemente de una que no.

### Test 3: Localidad temporal

Despues de cargar 65 páginas, el programa vuelve a acceder a las paginas 0 y 1 antes de provocar la carga de la página 64. Estas dos páginas representan el conjunto de páginas accedidas recientemente.

- FIFO: 69 accesos, 67 fallos, $2.90 \% $ de $Hit_{rate}$ y 3 reemplazos.
- LRU: 69 accesos, 65 fallos, $5.80 \% $ de $Hit_{rate}$ y 1 reemplazo.

FIFO expulsa primero la página 0 por ser la más antigua, aunque fue accedida recientemente. La lectura posterior de la pagina 0 falla y debe cargarla otra vez; ese reemplazo expulsa otra página antigua y tambien afecta la lectura posterior de la página 1.

LRU conserva las páginas 0 y 1 porque sus accesos las convierten en las páginas más accedidas recientemente. Solo se necesita un reemplazo para cargar la página 64 y las dos lecturas finales resultan hits.

## 4. Análisis

El caso mas representativo entre el cambio de $Hit_{rate}$ y reemplazoses es el caso 3 de las pruebas:

- Cambio de $Hit_{rate}$ de FIFO a LRU: $5.80 \% - 2.90 \% = 2.90 \text{puntos porcentuales}$.
- Hits adicionales: LRU obtiene 4 hits, frente a 2 hits de FIFO.
- Cambio de fallos: 65 frente a 67, es decir, 2 fallos menos.
- Cambio de reemplazos: 1 frente a 3, es decir, 2 reemplazos menos.
- Reducción relativa de reemplazos: $2 / 3 * 100 = 66.67 \% $.

En el caso 2 el $Hit_{rate}$ y la cantidad de reemplazos no cambian porque la secuencia es esencialmente un recorrido sin reutilización antes de volver a pedir una página ya expulsada. En el caso 1 tampoco hay diferencia observable: como todas las páginas caben, ninguna política tiene que reemplazar una página.

Los resultados muestran que la politica solo puede influir cuando hay se requiere aún más memoria. Si el conjunto activo cabe en RAM, o si el acceso no reutiliza páginas, el beneficio desaparece.

Teoricamente, LRU suele superar a FIFO cuando hay localidad temporal, porque reemplazaría la página cuyo próximo acceso está más lejano asumiendo que las accedidas más recientemente son una aproximación.