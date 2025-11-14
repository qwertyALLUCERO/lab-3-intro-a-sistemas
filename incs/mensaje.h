/*
 * mensaje.h - Definiciones compartidas para IPC con colas de mensajes
 */
#ifndef MENSAJE_H
#define MENSAJE_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

/* configuración de la fila de mensajes */
#define CLAVE_COLA 234
#define MAX_DATOS 100
#define TAM_MENSAJE 64

/* tipos de mensajes */
#define TIPO_DATO 1
#define TIPO_FIN 2

/* Estructura del mensaje */
struct mensaje_t 
{
    long tipo;
    int id_send;
    int numero_secuencia;
    int dato;
    char texto[TAM_MENSAJE];
    time_t timestamp;
};

/* funciones auxiliares */
int crear_cola(key_t clave);
void eliminar_cola(int msqid);
void imprimir_error(const char *msg);
int delay_aleatorio(int min_ms, int max_ms);

#endif /* MENSAJE_H */
