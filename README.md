# 💾 memory-os

Simulador de memoria virtual escrito en C++. Modela la traducción de direcciones virtuales, una tabla de páginas de dos niveles, memoria física y políticas de reemplazo FIFO/LRU.

El programa procesa un archivo de comandos, ejecuta las operaciones en orden y muestra estadísticas de accesos, fallos de página, reemplazos, Hit rate y tiempo de procesamiento.

- [Reporte de análisis - REPORTE.md](REPORTE.md)
- [Arquitectura](#arquitectura)
    - [Inversión de dependencias](#inversión-de-dependencias)
- [Estructura del proyecto](#estructura-del-proyecto)
- [Requisitos](#requisitos)
- [Compilación](#compilación)
- [Uso](#uso)
    - [Usar `make run`](#usar-make-run)
    - [Usar `bin/vmsim`](#usar-binvmsim)
    - [Opciones](#opciones)
- [Formato del archivo de comandos](#formato-del-archivo-de-comandos)
- [Pruebas](#pruebas)

## Arquitectura

[![](https://img.plantuml.biz/plantuml/svg/VLHDQm8n4BtdLmJlYeVIOr4AGg62rDABU9XcHmqccNIIRLbQ_tkpxgfnrtOlkpFlpSTx0pi94JYMreYeeq4vvOzIHrInP905IekMk1fI466ldGS6AoqvKZicYp9oYHaITb3GbtPRkG4JSiHeXx7oA8dZZZ90aVNaqCLbhD9yJgF1RKrUwwZ0W5xUZRCaWzlNoVkn41xK7hOe1-Tj1_9R22bN5hHRrrzdYdORXf3shYLYpPFfMJqQFULjMBSHMGkkc6c32-20tEONPBIgPg1Y6jFcNj0dJGlMBZQymzKyy3xLGDJapiEMsfO6KeybygTMsAyggDxyNF2cqut0J8qXLTUre-UsF2sMgbxXtU0ftbKXJJNpeo5DyhZW1XJAfxIpIHyssTv8QgfcUaDDtBPvvl8I7qJQiN5K3jEPtV0Xywq7z2tiKZfkTk6EiLsuRwBeTBnmSs5_aZAnej_zmkiVup_syKRY-CgKNyxLHJg7kuPl74xaukIezAEVcxnCtZ-i2RgY_Xdy0W00)](https://editor.plantuml.com/uml/VLHDQm8n4BtdLmJlYeVIOr4AGg62rDABU9XcHmqccNIIRLbQ_tkpxgfnrtOlkpFlpSTx0pi94JYMreYeeq4vvOzIHrInP905IekMk1fI466ldGS6AoqvKZicYp9oYHaITb3GbtPRkG4JSiHeXx7oA8dZZZ90aVNaqCLbhD9yJgF1RKrUwwZ0W5xUZRCaWzlNoVkn41xK7hOe1-Tj1_9R22bN5hHRrrzdYdORXf3shYLYpPFfMJqQFULjMBSHMGkkc6c32-20tEONPBIgPg1Y6jFcNj0dJGlMBZQymzKyy3xLGDJapiEMsfO6KeybygTMsAyggDxyNF2cqut0J8qXLTUre-UsF2sMgbxXtU0ftbKXJJNpeo5DyhZW1XJAfxIpIHyssTv8QgfcUaDDtBPvvl8I7qJQiN5K3jEPtV0Xywq7z2tiKZfkTk6EiLsuRwBeTBnmSs5_aZAnej_zmkiVup_syKRY-CgKNyxLHJg7kuPl74xaukIezAEVcxnCtZ-i2RgY_Xdy0W00)

El sistema sigue una arquitectura por capas y un flujo unidireccional:

1. `domain`. Contiene las reglas de memoria y no conoce archivos, argumentos ni salida por consola.

- `VirtualAddress`: separa una direccion en indíces PT1, PT2 y offset.
- `PageTable`: mantiene las entradas de la tabla de paginas de dos niveles.
- `PhysicalMemory`: representa los frames de la RAM simulada.
- `VirtualAllocator`: reserva y libera rangos de direcciones virtuales.
- `Mmu`: traduce direcciones, atiende fallos de pagina y coordina carga y reemplazo de paginas.
- `FifoPolicy` y `LruPolicy`: implementan algoritmos concretos de reemplazo.
- `Stats`: acumula accesos, fallos, reemplazos y tiempo.

2. `application`. Con `SimulatorService` coordina el caso de uso principal. Recibe comandos ya interpretados, ejecuta `alloc`, `read`, `write` y `free` mediante el allocator y la MMU, y devuelve las estadísticas finales. Esta capa no sabe cómo se obtuvieron los comandos ni cómo se presenta el reporte, por lo que puede probarse de forma independiente.

3. `infrastructure`. Contiene los adaptadores hacia el exterior, con `CliArgs` interpreta la configuración de la línea de comandos, `CommandFileParser` convierte el archivo de entrada en objetos `Command`, `ReplacementPolicyFactory` crea la política FIFO o LRU y `ReportPrinter` presenta las estadísticas al usuario.

### Inversión de dependencias

La MMU y el servicio de simulación no dependen directamente de `FifoPolicy` ni de `LruPolicy`. Ambos reciben una referencia a la abstracción `IReplacementPolicy`. El componente de alto nivel (`Mmu`) depende de un puerto estable, mientras que los algoritmos concretos dependen de ese contrato. Como resultado, se puede agregar otra política sin modificar la lógica de traducción de direcciones ni el servicio de aplicación.

## Estructura del proyecto

```text
include/                 Interfaces publicas
src/                     Implementacion de la aplicacion
  domain/                Reglas de memoria y politicas
  application/           Casos de uso
  infrastructure/        CLI, parser, factory y reportes
  main.cpp                 Archivo principal
tests/                   Pruebas unitarias y casos funcionales
tests/cases/             Archivos de entrada de escenarios
build/                   Objetos y dependencias generados
bin/                     Ejecutables generados
input.txt                Archivo predeterminado de comandos
```

## Requisitos

- Entorno compatible con `make` (como macOS o Linux)
- Compilador compatible con C++17
- Valgrind para la comprobacion de memoria

El `Makefile` busca primero `g++-16`, `g++-15`, `g++-14` y `g++-13`; si no encuentra ninguno, usa `g++`.

## Compilación

Desde la raiz del proyecto:

```shell
make all
```

El binario se genera en `bin/vmsim`. Los archivos intermedios se almacenan en `build/`.

Para eliminar los artefactos generados:

```shell
make clean
```

## Uso

### Usar `make run`

Esta opcion ejecuta automaticamente el archivo `input.txt` desde la raiz del proyecto. Puede ejecutarla con:

```shell
make run POLICY=lru PM=[512K]
```

- `POLICY` es opcional y permite cambiar de política de reemplazo entre `lru` y `fifo`. Si no es definda por defecto es `fifo`.

- `PM` es opcional y permite cambiar el tamaño de la memoria física por defecto de `256K`. Debe ser como mínimo `256K` y multiplo de `4096` bytes.

### Usar `bin/vmsim`

Esta opcion permite elegir cualquier archivo de comandos. La memoria física es opcional y, si se omite, el programa usa `256K` por defecto:

```shell
./bin/vmsim <archivo_comandos> [--policy=fifo|lru] [--phys-mem=<tamano>] [-v]
```

### Opciones

- `archivo_comandos`: archivo de texto con las operaciones de la simulacion. `input.txt` es el archivo incluido por defecto en el proyecto con `make run`.
- `POLICY` o `--policy=fifo|lru`: política de reemplazo. El valor predeterminado es `fifo`.
- `PM` o `--phys-mem=<tamaño>`: tamaño de la memoria física. Admite bytes o sufijos `K` y `M`, por ejemplo `262144`, `256K` o `1M`. El mínimo es `256K` y debe ser multiplo de `4096` bytes.


## Formato del archivo de comandos

Cada línea contiene una operación, las que se encuentran vacías o comienzan con `#` se ignoran. Las direcciones, tamaños y valores se interpretan como enteros decimales sin prefijo. Los accesos de lectura y escritura deben pertenecer a una asignación activa.

| Operación | Formato | Descripción |
| --- | --- | --- |
| `alloc` | `alloc <bytes>` | Reserva un rango de memoria virtual y muestra su dirección base. |
| `write` | `write <addr> <value>` | Escribe un byte, con `value` entre `0` y `255`. |
| `read` | `read <addr>` | Lee e imprime el byte almacenado. |
| `free` | `free <addr>` | Libera una asignación usando exactamente su dirección base. |

Por ejemplo:

```text
# Reservar 16 KB de memoria virtual
alloc 16384

# Escribir un byte en una dirección virtual
write 0 1
write 4096 2

# Leer un byte
read 0

# Liberar una asignación usando su dirección base
free 0
```

## Pruebas

Ejecutar las pruebas y los tres casos incluidos:

```shell
make test
```

El comando prueba:

- Comportamiento base cuando las páginas caben en memoria
- Reemplazo forzado con FIFO y LRU
- Localidad temporal para comparar ambas políticas.

Para revisar fugas y errores de memoria:

```shell
make valgrind
```

Si Valgrind no esta disponible, el `Makefile` compila y ejecuta una variante con `-fsanitize=address,undefined`.
