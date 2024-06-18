#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "cliente.c"
#include "servidor.c"

#define BUFFER_SIZE 1024

int main() {
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, start_server, NULL);

    char command[BUFFER_SIZE];
    while (1) {
        printf("> ");
        if (fgets(command, BUFFER_SIZE, stdin) == NULL) {
            perror("Error al leer el comando");
            continue;
        }

        if (strncmp(command, "open ", 5) == 0) { //Conectar
            char *ip_address = command + 5;
            ip_address[strcspn(ip_address, "\n")] = '\0';
            open_connection(ip_address);

        } else if (strncmp(command, "close", 5) == 0) {//Cerrar conexion
            close_connection();

        } else if (strncmp(command, "quit", 4) == 0) {//Salir
            close_connection();
            return 0;

        } else if (strncmp(command, "send ", 5) == 0) {
            //Enviar
            char *filename = command + 5;
            filename[strcspn(filename, "\n")] = '\0';
            send_file(filename);

        }else if (strncmp(command, "local ls", 8) == 0) {
            //Funcionalidad de listado de archivos del cliente
            list_local_files();

        }else if (strncmp(command, "remote ls", 9) == 0) {
            // Solicitar al servidor listado de archivos
            get_remote_files();
        }
        else {
            printf("Comando no reconocido: %s\n", command);
        }
    }
}
