/******** receive.c - Proceso que recibe y procesa datos de cola de mensajes
# *
# * Compilar: gcc -Iincs src/receive.c -o bin/receive
# * Ejecutar: ./bin/receive [velocidad_ms]
# */

#include "../incs/mensaje.h"

int main(int argc, char *argv[])
{
    int msqid;
    struct mensaje_t msg;
    int longitud = sizeof(struct mensaje_t) - sizeof(long);
    int contador = 0;
    int velocidad_proceso = 100; // ms defecto
    int send_activos = 1;
    int mensajes_fin = 0;
    
    /*saca la velocidad de procesamiento si se da*/
    if (argc > 1) 
    {
        velocidad_proceso = atoi(argv[1]);
    }
    
    printf("=== RECEIVE INICIADO ===\n");
    printf("Velocidad procesamiento: %d ms\n", velocidad_proceso);
    
    /*crea o conecta a la cola de mensajes */
    if ((msqid = msgget(CLAVE_COLA, IPC_CREAT | 0666)) == -1) 
    {
        perror("Error al crear/conectar cola de mensajes");
        exit(EXIT_FAILURE);
    }
    
    printf("consumidor conectado a cola %d\n", msqid);
    printf("esperando mensajes...\n\n");
    
    /* para recibir mensajes continuamente */
    while (1) 
    {
        /* recibir cualquier tipo de mensaje (0 = todos los tipos) */
        if (msgrcv(msqid, &msg, longitud, 0, 0) == -1) 
        {
            perror("Error al recibir mensaje");
            break;
        }
        
        if (msg.tipo == TIPO_FIN) 
        {
            mensajes_fin++;
            printf("\n>>> Recibido FIN del send %d <<<\n\n", msg.id_send);
            
            /* Si esperamos múltiples productores, verificar si todos terminaron */
            if (mensajes_fin >= send_activos) 
            {
                printf("todos los send han finalizado.\n");
                break;
            }
        } else 
        {
            /* procesar mensaje de datos */
            contador++;
            
            /* simula procesamiento */
            usleep(velocidad_proceso * 1000);
            
            printf("Recieve: [Msg %d] Prod=%d Seq=%d Dato=%d Texto=\"%s\"\n",
                   contador, msg.id_send, msg.numero_secuencia, 
                   msg.dato, msg.texto);
        }
    }
    
    printf("\n=== CONSUMIDOR FINALIZADO ===\n");
    printf("Total de mensajes procesados: %d\n", contador);
    
    /* Eliminar la cola de mensajes */
    if (msgctl(msqid, IPC_RMID, NULL) == -1) 
    {
        perror("error al eliminar cola de mensajes");
        exit(EXIT_FAILURE);
    }
    
    printf("cola de mensajes eliminada correctamente.\n");
    
    return EXIT_SUCCESS;
}