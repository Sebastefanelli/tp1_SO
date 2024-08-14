# include <stdio.h>
# include <stdlib.h>
# include <errno.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/sem.h>

int main(void){
    int semid= semget(0xa,4,IPC_CREAT|IPC_EXCL|0600);
    if(errno==EEXIST)
        printf("ya existe un conjunto de semaforos para la clave 0xa\n");
    else
        printf("semid = %d\n",semid);
    exit(0);
}
