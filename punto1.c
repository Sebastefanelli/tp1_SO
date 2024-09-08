#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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
  if (!str)
    return str;
  while (EOF != (ch = fgetc(fp)) && ch != '\n') {
    str[len++] = ch;
    if (len == size) {
      str = realloc(str, sizeof(char) * (size += 16));
      if (!str)
        return str;
    }
  }
  str[len++] = '\0';
  return realloc(str, sizeof(char) * len);
}

// Manejador para la señal SIGINT (Ctrl+C)
void myhandler(int sig) {
  printf(
      "\nRecibido SIGINT (Ctrl+C), esperando a que terminen los procesos...\n");
  for (int j = 0; j < pidarraycount; j++) {
    if (pidarray[j] != 0) {
      kill(pidarray[j], SIGKILL);    // Matar procesos en background
      waitpid(pidarray[j], NULL, 0); // Esperar a que terminen
      printf("Proceso %d terminado.\n", pidarray[j]);
    }
  }
  exit(0); // Finalizar el shell
}

// Manejador para la señal SIGTSTP (Ctrl+Z)
void myzhandler(int sig) {
  if (ignored == 0) {
    printf("Entrando en solo foreground (& es ignorado ahora)\n");
    ignored = 1;
  } else if (ignored == 1) {
    printf("Saliendo del modo solo foreground\n");
    ignored = 0;
  }
}

// Manejador para la señal SIGCHLD (procesos hijos terminados)
void sigchld_handler(int sig) {
  int saved_errno = errno;
  pid_t pid;
  int status;

  // Esperar a todos los procesos hijos que han terminado
  while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    if (WIFEXITED(status)) {
      printf("Proceso background %d terminó con estado de salida %d\n", pid,
             WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
      printf("Proceso background %d terminó por señal %d\n", pid,
             WTERMSIG(status));
    }
  }
  errno = saved_errno;
}

int main() {
  ignored = 0;
  struct sigaction sa;

  // Configuración para SIGINT
  sa.sa_handler = myhandler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);

  // Configuración para SIGTSTP
  sa.sa_handler = myzhandler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGTSTP, &sa, NULL);

  // Configuración para SIGCHLD
  struct sigaction sa_chld;
  sa_chld.sa_handler = sigchld_handler;
  sigemptyset(&sa_chld.sa_mask);
  sa_chld.sa_flags = SA_RESTART | SA_NOCLDSTOP;
  sigaction(SIGCHLD, &sa_chld, NULL);

  pidskilled = 0;
  pidarraycount = 0;
  char *line;
  int exitv = 0;

  while (exitv == 0) {
    skip = 0;
    printf("$ "); // Prompt del shell
    fflush(stdout);
    line = inputString(stdin, 10);

    if (strcmp("salir", line) == 0) {
      printf(
          "Saliendo, esperando a que terminen los procesos en background...\n");
      for (int j = 0; j < pidarraycount; j++) {
        if (pidarray[j] != 0) {
          kill(pidarray[j], SIGKILL);    // Matar procesos en background
          waitpid(pidarray[j], NULL, 0); // Esperar a que terminen
          printf("Proceso %d terminado.\n", pidarray[j]);
        }
      }
      free(line);
      break;
    }

    // Expande todas las apariciones de "$$" en la entrada del usuario
    char *pos;
    while ((pos = strstr(line, "$$")) != NULL) {
      char expandinput[200];
      int offset = pos - line;

      // Reemplaza $$ por el PID en la nueva cadena
      snprintf(expandinput, sizeof(expandinput), "%.*s%d%s", offset, line,
               getpid(), pos + 2);

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
    if (strcmp(args[0], "cd") == 0) {
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
        printf("terminado por la señal %d\n", exitstatus);
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
        if (args[i - 1] != NULL && strcmp(args[i - 1], "&") == 0 &&
            ignored == 0) {
          printf("El background pid es %d\n", spawnpid);
          pidarray[pidarraycount++] = spawnpid;
          numpids++;
        } else {
          waitpid(spawnpid, &exitstatus, 0);
          if (WIFEXITED(exitstatus)) {
            exitstatus = WEXITSTATUS(exitstatus);
          } else if (WIFSIGNALED(exitstatus)) {
            exitstatus = WTERMSIG(exitstatus);
            printf("Terminado por la señal %d\n", exitstatus);
          }
        }
      }
    }
    free(line);
  }
  return 0;
}
