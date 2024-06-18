#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8889
#define BUFFER_SIZE 1024

int client_socket = -1;  // Socket del cliente global para manejar la conexión actual


void list_local_files() {
    system("ls");
}

void get_remote_files(){
    if (client_socket == -1) {
        printf("No hay conexión activa. Use 'open <direccion-ip>' para iniciar una conexión.\n");
        return;
    }
    
    // Enviar instrucción "ls" al servidor
    send(client_socket, "ls", strlen("ls"), 0);

    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    // Recibir y mostrar los nombres de los archivos/directorios
    printf("Archivos en el servidor:\n");
    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        if (strcmp(buffer, "EOT") == 0)
        {
            break;
        }
        
        // Agregar un terminador nulo al final de los datos recibidos
        buffer[bytes_received] = '\0';

        // Mostrar el nombre del archivo/directorio
        printf("%s", buffer);
    }

    if (bytes_received < 0) {
        perror("Error al recibir datos del servidor");
    }
}

void send_file(const char *filename) {
    FILE *file;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    if (client_socket == -1) {
        printf("No hay conexión activa. Use 'open <direccion-ip>' para iniciar una conexión.\n");
        return;
    }

    // Enviar el nombre del archivo
    send(client_socket, filename, sizeof(filename), 0);

    // Abrir el archivo para leer
    file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return;
    }

    // Enviar el archivo
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        send(client_socket, buffer, bytes_read, 0);
    }

    fclose(file);
    printf("Archivo enviado con éxito: %s\n", filename);
}

void open_connection(const char *ip_address) {
    struct sockaddr_in server_addr;

    // Crear el socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("No se pudo crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, ip_address, &server_addr.sin_addr) <= 0) {
        perror("Dirección IP inválida");
        close(client_socket);
        client_socket = -1;
        return;
    }

    // Conectar al servidor
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error al conectar con el servidor");
        close(client_socket);
        client_socket = -1;
        return;
    }

    printf("Conexión establecida con %s\n", ip_address);
}

void close_connection() {
    if (client_socket != -1) {
        close(client_socket);
        client_socket = -1;
        printf("Conexión cerrada\n");
    } else {
        printf("No hay conexión activa para cerrar\n");
    }
}
