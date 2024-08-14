#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

int pidarray[1000];
int pidarraycount;
int numpids;
int exitstatus;
int lastforeground;
int pidskilled;
int skip;
int ignored;
static volatile int keeprunning = 1;

// Estructura para leer la entrada del usuario
char *inputString(FILE *fp, size_t size) {
    char *str;
    int ch;
    size_t len = 0;
    str = realloc(NULL, sizeof(char) * size);
    if (!str) return str;
    while (EOF != (ch = fgetc(fp)) && ch != '\n') {
        str[len++] = ch;
        if (len == size) {
            str = realloc(str, sizeof(char) * (size += 16));
            if (!str) return str;
        }
    }
    str[len++] = '\0';
    return realloc(str, sizeof(char) * len);
}

// Manejador para la señal SIGINT (Ctrl+C)
void myhandler(int dummy) {
    printf("terminado por signal %d\n", dummy);
}

// Manejador para la señal SIGTSTP (Ctrl+Z)
void myzhandler(int dummy) {
    if (ignored == 0) {
        printf("Entrando en solo foreground (& es ignorado ahora)\n");
        ignored = 1;
    } else if (ignored == 1) {
        printf("saliendo del modo solo foreground\n");
        ignored = 0;
    }
}

int main() {
    ignored = 0;
    struct sigaction sigih;
    sigih.sa_handler = myhandler;
    sigemptyset(&sigih.sa_mask);
    sigih.sa_flags = 0;
    sigaction(SIGINT, &sigih, NULL);
    sigih.sa_handler = myzhandler;
    sigemptyset(&sigih.sa_mask);
    sigih.sa_flags = 0;
    sigaction(SIGTSTP, &sigih, NULL);

    pidskilled = 0;
    pidarraycount = 0;
    char *line;
    int exitv = 0;
    int exitsignal = 0;
    int terminatedby = 0;

    while (exitv == 0) {
        skip = 0;

        // Verifica si los procesos en segundo plano han terminado
        int j = 0;
        int status2;
        if (numpids > 0) {
            for (j = 0; j < pidarraycount; j++) {
                pid_t result = waitpid(pidarray[j], &status2, WNOHANG);
                if (pidarray[j] != 0) {
                    if (result != 0) {
                        printf("background pid %d termino: valor de salida  %d\n", result, status2);
                        kill(pidarray[j], SIGKILL);
                        pidskilled++;
                        pidarray[j] = 0;
                    }
                }
            }
        }
        printf("$"); // Inicio del shell
        line = inputString(stdin, 10);
        fflush(stdin);
        fflush(stdout);

        if (strcmp("exit $", line) == 0) {
            exitv++;
        }

        // Expande todas las apariciones de "$$" en la entrada del usuario
        char *pos;
        while ((pos = strstr(line, "$$")) != NULL) {
            char expandinput[200];
            int offset = pos - line;

            // Reemplaza $$ por el PID en la nueva cadena
            snprintf(expandinput, sizeof(expandinput), "%.*s%d%s", offset, line, getpid(), pos + 2);

            // Copia la nueva cadena de vuelta a line
            strncpy(line, expandinput, 50);
        }

        // Divide la entrada en tokens
        char *args[512];
        char *token = strtok(line, " ");
        int i = 0;
        while (token != NULL) {
            args[i] = token;
            i++;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        if (args[0] == NULL) {
            continue; // Si no hay entrada, vuelve a pedir
        }

        // Comandos internos
        if (strcmp(args[0], "exit") == 0) {
            exitv++;
        } else if (strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL) {
                const char *home = getenv("HOME");
                chdir(home);
            } else {
                if (chdir(args[1]) != 0) {
                    perror("chdir");
                }
            }
        } else if (strcmp(args[0], "status") == 0) {
            if (lastforeground != 1) {
                printf("exit value %d\n", exitstatus);
            } else {
                printf("terminado por la señal %d\n", terminatedby);
            }
        } else if (args[0][0] != '#') {
            pid_t spawnpid = fork();
            if (spawnpid == -1) {
                perror("fork");
                exit(1);
            } else if (spawnpid == 0) {
                execvp(args[0], args);
                perror("execvp");
                exit(1);
            } else {
                if (args[i - 1] != NULL && strcmp(args[i - 1], "&") == 0 && ignored == 0) {
                    printf("el background pid es  %d\n", spawnpid);
                    pidarray[pidarraycount++] = spawnpid;
                    numpids++;
                } else {
                    waitpid(spawnpid, &exitstatus, 0);
                    if (WIFEXITED(exitstatus)) {
                        exitstatus = WEXITSTATUS(exitstatus);
                    } else if (WIFSIGNALED(exitstatus)) {
                        exitstatus = WTERMSIG(exitstatus);
                        printf("terminado por la señal %d\n", exitstatus);
                    }
                }
            }
        }
        free(line);
    }
    return 0;
}

