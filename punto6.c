#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
        return 1;
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {  // Proceso hijo
        close(pipefd[0]);  // Cerrar lectura
        dup2(pipefd[1], STDOUT_FILENO);  // Redirigir stdout al pipe
        close(pipefd[1]);

        execvp(argv[1], &argv[1]);
        perror("execvp");  // Si execvp falla
        exit(1);
    } else {  // Proceso padre
        close(pipefd[1]);  // Cerrar escritura

        char buffer[1024];
        int bytes_read;
        while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';  // Null-terminar la cadena
            printf("%s", buffer);
        }

        close(pipefd[0]);

        int status;
        waitpid(pid, &status, 0);  // Esperar a que el hijo termine
    }

    return 0;
}

