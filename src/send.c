/*
 * send.c - Proceso que genera y envía datos
 * 
 * Compilar: gcc -Iincs src/send.c -o bin/send
 * Ejecutar: ./bin/send [id_send] [num_mensajes]
 */

#include "../incs/mensaje.h"
#include "../incs/random.h"

int main(int argc, char *argv[]) 
{
    srand(time(NULL) + getpid());
    
    struct mensaje_t m;
    int msg_id, msg_count = 1;
    int id_send = 1;
    int total_messages = MAX_DATOS;
    int longitud = sizeof(struct mensaje_t) - sizeof(long);
    
    // Parsear argumentos
    if (argc > 1) 
    {
        id_send = atoi(argv[1]);
    }
    if (argc > 2) 
    {
        total_messages = atoi(argv[2]);
    }
    
    printf("=== SEND %d INICIADO ===\n", id_send);
    printf("PID: %d\n", getpid());
    printf("Mensajes a enviar: %d\n", total_messages);
    
    // Crear la cola de mensajes (usando CLAVE_COLA)
    msg_id = msgget(CLAVE_COLA, 0666 | IPC_CREAT);
    if (msg_id == -1) 
    {
        perror("Error al crear la cola de mensajes");
        return 1;
    }
    
    printf("Cola creada/conectada: %d\n\n", msg_id);
    
    // Enviar mensajes
    while (msg_count <= total_messages) 
    {
        // Delay aleatorio (50-500ms como pide el lab)
        int delay_ms = 50 + (rand() % 451);
        usleep(delay_ms * 1000);
        
        // Preparar mensaje
        m.tipo = TIPO_DATO;
        m.id_send = id_send;
        m.numero_secuencia = msg_count;
        m.dato = randomInt(1, 1000);
        m.timestamp = time(NULL);
        
        // Mensaje alfanumérico con letra aleatoria
        snprintf(m.texto, TAM_MENSAJE, "Send%d-Msg%03d-%c-Dato:%04d", 
                id_send, msg_count, letraRandom(), m.dato);
        
        // Enviar el mensaje
        if (msgsnd(msg_id, &m, longitud, 0) == -1)
        {
            perror("Error al enviar el mensaje");
            return 1;
        }
        
        printf("[%3d/%3d] Enviado: %s (delay=%dms)\n",
               msg_count, total_messages, m.texto, delay_ms);
        
        msg_count++;
    }
    
    // Enviar mensaje de finalización
    m.tipo = TIPO_FIN;
    m.id_send = id_send;
    m.numero_secuencia = -1;
    m.dato = -1;
    m.timestamp = time(NULL);
    snprintf(m.texto, TAM_MENSAJE, "FIN-Send%d", id_send);
    
    if (msgsnd(msg_id, &m, longitud, 0) == -1) 
    {
        perror("Error al enviar mensaje FIN");
        return 1;
    }
    
    printf("\n=== SEND %d FINALIZADO ===\n", id_send);
    printf("Total enviado: %d mensajes + 1 FIN\n", total_messages);
    
    return 0;
}