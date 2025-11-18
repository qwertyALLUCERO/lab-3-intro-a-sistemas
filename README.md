# Laboratorio 3 : Comunicación entre Procesos

**Universidad de Magallanes**  
**Sistemas Operativos**
**Rodolfo Aguilar / Alexander Lucero / Benjamin Reyes **

## Descripción

Implementación del problema Send-Receive utilizando colas de mensajes  (`msgget`, `msgsnd`, `msgrcv`, `msgctl`).

## Estructura del Proyecto

```
lab3/
├── bin/             # Ejecutables (generado)
├── logs/            # Registros de los procesos generados por el Makefile
├── src/
│   ├── send.c       # Proceso que genera datos
│   └── receive.c    # Proceso que consume datos
├── incs/
│   └── mensaje.h    # Definiciones compartidas
│   └── random.h     # no usado
├── docs/            # Archivos relacionados al informe
├── Makefile         # Automatización de compilación
└── README.md        # Este archivo
```

## Compilación

```bash
# Compilar todo
make

# Compilar solo send
make send

# Compilar solo receive
make receive
```

## Ejecución

### Experimento Básico (1 Productor → 1 Consumidor)

```bash
# Terminal 1: Iniciar send
./bin/send &

# Terminal 2: Iniciar receive
./bin/receive
```

### Experimentos de Análisis

#### A) Consumidor LENTO (más mensajes que capacidad de atención)
```bash
make test-slow-consumer
# Consumidor procesa cada mensaje en 500ms
# El productor genera más rápido → buffer se llena
```

#### B) Consumidor RÁPIDO (procesa más rápido que producción)
```bash
make test-fast-consumer
# Consumidor procesa cada mensaje en 10ms
# El productor genera más lento → buffer vacío
```

## Monitoreo del Sistema

```bash
# Ver colas de mensajes activas
make status
# o directamente:
ipcs

# Limpiar colas manualmente
make cleanq
# o:
ipcrm -q [msqid]
```

## Características Implementadas

- Productor genera 100 datos con retardo aleatorio (50-500ms)
- Consumidor con velocidad configurable
- Mensajes alfanuméricos con timestamp
- Control de finalización con mensaje especial
- Información de secuencia y origen

## Comandos Útiles

```bash
# Ver procesos activos
ps aux | grep send
ps aux | grep receive

# Matar procesos
killall send
killall receive

# Limpiar todo
make clean
make cleanq
```

## RECORDATORIOS

0. Instrucciones adicionales con el comando `Make help`  
1. Siempre eliminar colas al finalizar (`msgctl(msqid, IPC_RMID, NULL)`)
2. Usar `ipcs` para verificar recursos IPC activos
3. El sistema tiene límites en cantidad de colas y tamaño de buffer
4. Los permisos 0666 permiten acceso compartido entre usuarios

## REFERENCIAS

- `man msgget`
- `man msgsnd`
- `man msgrcv`
- `man msgctl`
- `man ipcs`
