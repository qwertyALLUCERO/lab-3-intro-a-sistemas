/*
 * productor.c - Proceso que genera y envía datos a través de cola de mensajes
 * 
 * Compilar: gcc -Iinc src/productor.c -o bin/productor
 * Ejecutar: ./bin/productor [id_send]
 */

#include "../incs/mensaje.h"

int main(int argc, char *argv[]) 
{
    int msqid;
    struct mensaje_t msg;
    int id_send = 1;
    int longitud = sizeof(struct mensaje_t) - sizeof(long);
    
    /* saca el ID del productor */
    if (argc > 1)
    {
        id_send = atoi(argv[1]);
    }
    
    printf("=== SEND %d INICIADO ===\n", id_send);
    
    /* Crear o conectar a la cola de mensajes */
    if ((msqid = msgget(CLAVE_COLA, IPC_CREAT | 0666)) == -1) {
        perror("Error al crear/conectar cola de mensajes");
        exit(EXIT_FAILURE);
    }
    
    printf("Productor %d conectado a cola %d\n", id_send, msqid);
    
    /* inicializar generador de números aleatorios */
    srand(time(NULL) + id_send);
    
    /* Producir y enviar MAX_DATOS mensajes */
    for (int i = 0; i < MAX_DATOS; i++)
    {
        /* preparar el mensaje */
        msg.tipo = TIPO_DATO;
        msg.id_send = id_send;
        msg.numero_secuencia = i + 1;
        msg.dato = rand() % 1000; // dato aleatorio entre 0-999
        msg.timestamp = time(NULL);
        snprintf(msg.texto, TAM_MENSAJE, "Dato#%d del Prod%d", i+1, id_send);
        
        /* delay aleatorio de producción (50-500 ms) */
        int delay = 50000 + (rand() % 450000);
        usleep(delay);
        
        /*enviar mesjaje */
        if (msgsnd(msqid, &msg, longitud, 0) == -1)
        {
            perror("Error al enviar mensaje");
            exit(EXIT_FAILURE);
        }
        
        printf("Productor %d: Enviado [%d/%d] dato=%d (delay=%dms)\n", 
               id_send, i+1, MAX_DATOS, msg.dato, delay/1000);
    }
    
    /* Enviar mensaje de finalización */
    msg.tipo = TIPO_FIN;
    msg.id_send = id_send;
    msg.numero_secuencia = -1;
    strcpy(msg.texto, "FIN");
    
    if (msgsnd(msqid, &msg, longitud, 0) == -1) {
        perror("Error al enviar mensaje FIN");
        exit(EXIT_FAILURE);
    }
    
    printf("=== PRODUCTOR %d FINALIZADO ===\n", id_send);
    printf("Total mensajes enviados: %d + 1 FIN\n", MAX_DATOS);
    
    return EXIT_SUCCESS;
}
