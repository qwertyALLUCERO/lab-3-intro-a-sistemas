# Makefile para Laboratorio 3 - IPC con Colas de Mensajes
# Universidad de Magallanes - Sistemas Operativos

CC = gcc
CFLAGS = -Wall -Iincs
SRCDIR = src
INCDIR = incs
BINDIR = bin
LOGDIR = logs

$(shell mkdir -p $(BINDIR) $(LOGDIR))

HEADERS = $(INCDIR)/random.h $(INCDIR)/mensaje.h

# ═══════════════════════════════════════════
# COMPILACION
# ═══════════════════════════════════════════

all: send receive
	@echo " Compilacion exitosa"

send: $(SRCDIR)/send.c $(HEADERS)
	$(CC) $(CFLAGS) $(SRCDIR)/send.c -o $(BINDIR)/send

receive: $(SRCDIR)/receive.c $(HEADERS)
	$(CC) $(CFLAGS) $(SRCDIR)/receive.c -o $(BINDIR)/receive

# ═══════════════════════════════════════════
# LIMPIEZA
# ═══════════════════════════════════════════

clean:
	rm -rf $(BINDIR)/* $(LOGDIR)/*
	@ipcrm -Q 12345 2>/dev/null || true
	@echo " Limpieza completa"

cleanq:
	@ipcrm -Q 12345 2>/dev/null || true
	@echo " Colas eliminadas"

# ═══════════════════════════════════════════
# MONITOREO
# ═══════════════════════════════════════════

status:
	@echo "=== Colas de mensajes ==="
	@ipcs -q
	@echo ""
	@echo "=== Procesos activos ==="
	@ps aux | grep "bin/send\|bin/receive" | grep -v grep || echo "Sin procesos"

kill:
	@killall send receive 2>/dev/null || true
	@make cleanq
	@echo " Procesos terminados"

# ═══════════════════════════════════════════
# PRUEBAS (Tu estilo: simple y directo)
# ═══════════════════════════════════════════

# Test basico: 1 send → 1 receive (100 mensajes)
test: all cleanq
	@echo "=== TEST BASICO: 100 MENSAJES ==="
	$(BINDIR)/receive normal 1 > $(LOGDIR)/test-receive.log 2>&1 & 
	@sleep 2
	$(BINDIR)/send 1 100 | tee $(LOGDIR)/test-send.log
	@echo ""
	@echo " Logs en: $(LOGDIR)/test-*.log"

# Analisis A: Buffer lleno (receive LENTO)
test-slow: all cleanq
	@echo "=== ANALISIS A: BUFFER LLENO ==="
	@echo "Receive procesa en 2000ms, Send genera cada 50-500ms"
	$(BINDIR)/receive lento 1 > $(LOGDIR)/slow-receive.log 2>&1 & 
	@sleep 2
	$(BINDIR)/send 1 50 | tee $(LOGDIR)/slow-send.log
	@echo ""
	@echo " Logs en: $(LOGDIR)/slow-*.log"

# Analisis B: Buffer vacio (receive RAPIDO)
test-fast: all cleanq
	@echo "=== ANALISIS B: BUFFER VACIO ==="
	@echo "Receive procesa instantaneo, Send genera cada 50-500ms"
	$(BINDIR)/receive rapido 1 > $(LOGDIR)/fast-receive.log 2>&1 & 
	@sleep 2
	$(BINDIR)/send 1 50 | tee $(LOGDIR)/fast-send.log
	@echo ""
	@echo " Logs en: $(LOGDIR)/fast-*.log"

# Multiples productores: 3 sends → 1 receive
test-multi: all cleanq
	@echo "=== 3 SENDS → 1 RECEIVE ==="
	$(BINDIR)/receive normal 3 > $(LOGDIR)/multi-receive.log 2>&1 & 
	@sleep 2
	$(BINDIR)/send 1 30 > $(LOGDIR)/multi-send1.log 2>&1 & 
	$(BINDIR)/send 2 30 > $(LOGDIR)/multi-send2.log 2>&1 &
	$(BINDIR)/send 3 30 > $(LOGDIR)/multi-send3.log 2>&1 &
	@echo "3 senders lanzados..."
	@sleep 35
	@echo ""
	@echo " Logs en: $(LOGDIR)/multi-*.log"

# 10 productores simultaneos (PREGUNTA DEL LAB)
test-10: all cleanq
	@echo "=== 10 SENDS → 1 RECEIVE (PREGUNTA) ==="
	$(BINDIR)/receive normal 10 > $(LOGDIR)/test10-receive.log 2>&1 & 
	@sleep 2
	@for i in 1 2 3 4 5 6 7 8 9 10; do \
		$(BINDIR)/send $$i 15 > $(LOGDIR)/test10-send$$i.log 2>&1 & \
	done
	@echo "10 senders lanzados..."
	@sleep 25
	@echo ""
	@echo " Logs en: $(LOGDIR)/test10-*.log"
	@echo "Analiza el orden en: $(LOGDIR)/test10-receive.log"

# Ejecutar TODOS los experimentos
test-all: test test-slow test-fast test-multi test-10
	@echo ""
	@echo "═══════════════════════════════════════════"
	@echo "   TODOS LOS EXPERIMENTOS COMPLETADOS"
	@echo "═══════════════════════════════════════════"
	@ls -lh $(LOGDIR)/

# Ver logs
logs:
	@ls -lh $(LOGDIR)/

# ═══════════════════════════════════════════
# AYUDA
# ═══════════════════════════════════════════

help:
	@echo ""
	@echo "COMANDOS DISPONIBLES:"
	@echo ""
	@echo "  make              - Compilar"
	@echo "  make clean        - Limpiar todo"
	@echo "  make cleanq       - Limpiar colas"
	@echo "  make status       - Ver estado IPC"
	@echo "  make kill         - Matar procesos"
	@echo ""
	@echo "PRUEBAS:"
	@echo "  make test         - 100 mensajes (REQUISITO)"
	@echo "  make test-slow    - Analisis A (buffer lleno)"
	@echo "  make test-fast    - Analisis B (buffer vacio)"
	@echo "  make test-multi   - 3 sends → 1 receive"
	@echo "  make test-10      - 10 sends (PREGUNTA)"
	@echo "  make test-all     - TODOS"
	@echo ""
	@echo "USO MANUAL:"
	@echo "  ./bin/receive &"
	@echo "  ./bin/send"
	@echo ""

.PHONY: all clean cleanq status kill test test-slow test-fast test-multi test-10 test-all logs help