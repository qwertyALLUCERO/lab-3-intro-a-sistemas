/*
 * receive.c - Proceso que recibe y procesa datos
 * 
 * Compilar: gcc -Iincs src/receive.c -o bin/receive
 * Ejecutar: ./bin/receive [modo] [num_senders]
 * 
 * Modos:
 *   lento  - Procesa lento (2000ms) - Análisis A
 *   rapido - Procesa rápido (0ms)   - Análisis B
 *   normal - Velocidad estándar (500ms)
 */

#include "../incs/mensaje.h"

int main(int argc, char *argv[])
{
    int msqid;
    struct mensaje_t msg;
    int longitud = sizeof(struct mensaje_t) - sizeof(long);
    int contador = 0;
    int send_activos = 1;
    int mensajes_fin = 0;
    
    // Configuración de velocidad de procesamiento
    int delay_ms = 500;  // Normal por defecto
    char modo[20] = "normal";
    
    // Parsear argumentos
    if (argc > 1) 
    {
        strncpy(modo, argv[1], sizeof(modo) - 1);
        modo[sizeof(modo) - 1] = '\0';
        
        if (strcmp(modo, "lento") == 0) 
        {
            delay_ms = 2000;  // 2 segundos
        } 
        else if (strcmp(modo, "rapido") == 0) 
        {
            delay_ms = 0;  // Sin delay
        }
    }
    
    if (argc > 2) 
    {
        send_activos = atoi(argv[2]);
    }
    
    printf("=== RECEIVE INICIADO ===\n");
    printf("PID: %d\n", getpid());
    printf("Modo: %s (delay=%dms)\n", modo, delay_ms);
    printf("Esperando mensajes de %d send(s)...\n\n", send_activos);
    
    // Crear o conectar a la cola de mensajes (usando CLAVE_COLA)
    msqid = msgget(CLAVE_COLA, IPC_CREAT | 0666);
    if (msqid == -1) 
    {
        perror("Error al crear/conectar cola de mensajes");
        exit(EXIT_FAILURE);
    }
    
    printf("Conectado a cola: %d\n\n", msqid);
    
    time_t inicio = time(NULL);
    
    // Recibir mensajes continuamente
    while (1) 
    {
        // Recibir cualquier tipo de mensaje (0 = todos los tipos)
        if (msgrcv(msqid, &msg, longitud, 0, 0) == -1) 
        {
            perror("Error al recibir mensaje");
            break;
        }
        
        if (msg.tipo == TIPO_FIN) 
        {
            mensajes_fin++;
            printf("\n>>> FIN recibido del Send %d [%d/%d] <<<\n\n", 
                   msg.id_send, mensajes_fin, send_activos);
            
            // Si todos los senders terminaron
            if (mensajes_fin >= send_activos) 
            {
                printf("Todos los sends han finalizado.\n");
                break;
            }
        } 
        else 
        {
            // Procesar mensaje de datos
            contador++;
            
            // Simular procesamiento con delay
            if (delay_ms > 0) 
            {
                usleep(delay_ms * 1000);
            }
            
            printf("[%3d] Recibido: %s\n", contador, msg.texto);
            
            // Detalles cada 10 mensajes
            if (contador % 10 == 0) 
            {
                printf("    └─ Send:%d Sec:%d Dato:%d\n",
                       msg.id_send, msg.numero_secuencia, msg.dato);
            }
        }
    }
    
    time_t fin = time(NULL);
    int tiempo_total = (int)difftime(fin, inicio);
    
    printf("\n=== RECEIVE FINALIZADO ===\n");
    printf("Total mensajes procesados: %d\n", contador);
    printf("Tiempo total: %d segundos\n", tiempo_total);
    if (tiempo_total > 0) 
    {
        printf("Velocidad promedio: %.2f msg/s\n", (float)contador / tiempo_total);
    }
    
    // Eliminar la cola de mensajes
    printf("\nEliminando cola de mensajes...\n");
    if (msgctl(msqid, IPC_RMID, NULL) == -1) 
    {
        perror("Error al eliminar cola de mensajes");
        exit(EXIT_FAILURE);
    }
    
    printf("Cola eliminada correctamente.\n");
    
    return EXIT_SUCCESS;
}
