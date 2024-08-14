#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define pids "pids.txt"

int main(void) {
    FILE *pid_file = fopen(pids, "r");
    if (pid_file == NULL) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    pid_t pid;
    int signal_number;
    // Leer y enviar señales para cada PID en el archivo
    while (fscanf(pid_file, "%d", &pid) != EOF) {
        printf("PID leído: %d\n", pid);

        printf("Ingrese la señal que desea enviar 2 para SIGINT y 10 para SIGUSR1 al proceso con PID %d: ", pid);
        scanf("%d", &signal_number);

        while (getchar() != '\n'); // Limpiar el buffer

        kill(pid, signal_number);
        printf("Señal %d enviada al proceso con PID %d\n", signal_number, pid);
    }

    fclose(pid_file);
    return 0;
}

