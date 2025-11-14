# Makefile para Laboratorio de IPC con Colas de Mensajes
# Universidad de Magallanes - Sistemas Operativos

CC = gcc
CFLAGS = -Wall -Iincs
SRCDIR = src
INCDIR = incs
BINDIR = bin

# Crear directorio bin si no existe
$(shell mkdir -p $(BINDIR))

all: send receive

send: $(SRCDIR)/send.c $(INCDIR)/mensaje.h
	$(CC) $(CFLAGS) $(SRCDIR)/send.c -o $(BINDIR)/send

receive: $(SRCDIR)/receive.c $(INCDIR)/mensaje.h
	$(CC) $(CFLAGS) $(SRCDIR)/receive.c -o $(BINDIR)/receive

# Limpiar binarios
clean:
	rm -rf $(BINDIR)/*
	ipcrm -q $(shell ipcs -q | grep $(USER) | awk '{print $$2}') 2>/dev/null || true

# Limpiar colas de mensajes manualmente
cleanq:
	@echo "Limpiando colas de mensajes..."
	@ipcs -q | grep $(USER) || echo "No hay colas activas"
	@ipcrm -q $(shell ipcs -q | grep $(USER) | awk '{print $$2}') 2>/dev/null || echo "Colas limpiadas"

# Ejecutar experimento básico
test: all
	@echo "=== Experimento 1: send-receive básico ==="
	@echo "Iniciando receive en background..."
	./$(BINDIR)/receive &
	@sleep 1
	@echo "Iniciando send..."
	./$(BINDIR)/send

# experimento: receive lento (buffer lleno)
test-slow-consumer: all
	@echo "=== Experimento 2: receive LENTO (análisis A) ==="
	./$(BINDIR)/receive 500 &
	@sleep 1
	./$(BINDIR)/send

# experimento: receive rápido (buffer vacío)
test-fast-consumer: all
	@echo "=== Experimento 3: receive RÁPIDO (análisis B) ==="
	./$(BINDIR)/receive 10 &
	@sleep 1
	./$(BINDIR)/send

# Ver estado de colas
status:
	@echo "=== Estado de IPC ==="
	@ipcs

.PHONY: all clean cleanq test test-slow-consumer test-fast-consumer status
