#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void* handle_client(void* client_socket_ptr) {
    int client_socket = *(int*)client_socket_ptr;
    free(client_socket_ptr);
    
    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, sizeof(buffer));

        // Leer el comando enviado por el cliente
        int bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_read <= 0) {
            break; // Error o conexión cerrada
        }

        buffer[bytes_read] = '\0';

        // Remover el carácter '\r' si está presente
        buffer[strcspn(buffer, "\r")] = 0;

        // Si el comando es "salir", cerrar la conexión
        if (strcmp(buffer, "salir") == 0) {
            printf("Cliente desconectado.\n");
            break;
        }

        // Crear un pipe para capturar la salida del comando
        int pipefd[2];
        if (pipe(pipefd) == -1) {
            perror("pipe");
            close(client_socket);
            return NULL;
        }

        pid_t pid = fork();
        if (pid == 0) {  // Proceso hijo
            close(pipefd[0]);  // Cerrar lectura del pipe
            dup2(pipefd[1], STDOUT_FILENO);  // Redirigir stdout al pipe
            dup2(pipefd[1], STDERR_FILENO);  // Redirigir stderr al pipe
            close(pipefd[1]);  // Cerrar escritura del pipe

            // Ejecutar el comando
            execlp("/bin/sh", "sh", "-c", buffer, (char*)NULL);
            perror("execlp");  // En caso de error
            exit(1);
        } else if (pid > 0) {  // Proceso padre
            close(pipefd[1]);  // Cerrar escritura del pipe

            // Leer la salida del comando y enviarla al cliente
            while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
                buffer[bytes_read] = '\0';
                send(client_socket, buffer, bytes_read, 0);
            }

            close(pipefd[0]);  // Cerrar lectura del pipe
            wait(NULL);  // Esperar a que el hijo termine
        } else {
            perror("fork");
            close(client_socket);
            return NULL;
        }
    }

    close(client_socket);
    return NULL;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Crear el socket del servidor
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Configurar la estructura del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Vincular el socket a la dirección y puerto
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Escuchar conexiones entrantes
    if (listen(server_socket, 3) < 0) {
        perror("listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Servidor escuchando en el puerto %d...\n", PORT);

    while (1) {
        // Aceptar una conexión entrante
        if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len)) < 0) {
            perror("accept");
            continue;
        }

        printf("Cliente conectado.\n");

        // Crear un nuevo hilo para manejar la conexión del cliente
        pthread_t thread_id;
        int* new_client_socket = malloc(sizeof(int));
        *new_client_socket = client_socket;
        if (pthread_create(&thread_id, NULL, handle_client, new_client_socket) != 0) {
            perror("pthread_create");
            close(client_socket);
            free(new_client_socket);
        }

        // Detach el hilo para que libere sus recursos al terminar
        pthread_detach(thread_id);
    }

    close(server_socket);
    return 0;
}

