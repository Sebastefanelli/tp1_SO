#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define pids "pids.txt"

void signal_handler(int sigNUM){
    printf("el proceso hijo con PID: %d recibió la señal SIGUSR1 \n ", getpid());
    exit(getpid() % 10);
}
void signal_handler_2(int sigNUM2){
    printf("el proceso hijo con PID: %d recibió la señal SIGINT \n ", getpid());
    exit(getpid() % 10);
}

int main (int argc, char * argv[]){
    int numhijos=atoi(argv[1]);
    int pid_file;

    /*printf("ingrese la cantidad de procesos que desea crear:");
    scanf("%d",&numhijos);
    while (getchar() != '\n');*/

    pid_t vector[numhijos];
    int suma=0;
    int status;
    printf("\nCreando %d procesos hijos...\n", numhijos);
    pid_file= open(pids, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    for (int i = 0; i < numhijos; i++) {
        pid_t pid = fork();
        if(pid<0){
            printf("error en fork");
        }else if (pid==0){
            signal(SIGUSR1, signal_handler);
            signal(SIGINT, signal_handler_2);
            printf("el hijo con PID: %d entra en pausa\n",getpid());
            pause();
            exit(EXIT_SUCCESS);
        }else{
            vector[i]=pid;
            dprintf(pid_file, "%d\n", pid);
        }

    }
    close(pid_file);
    for (int i = 0; i < numhijos; i++) {
        waitpid(vector[i], &status, 0);
        if (WIFEXITED(status)) { // verifico si el proceso ha sido terminado normalmente
            int retorno_hijo = WEXITSTATUS(status);
            printf("Hijo %d retornó: %d\n", vector[i], retorno_hijo);
            suma+= retorno_hijo;
        }
    }
    printf("La suma de los valores retornados por los hijos es: %d\n", suma);
    return 0;
}
