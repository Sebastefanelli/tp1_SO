# include <stdio.h>
# include <stdlib.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/sem.h>

int main(void){
    int semid= semget(0xa,0,0);
    if (semid!=-1){
        semctl(semid,0,SETVAL,1);//semA
        semctl(semid,1,SETVAL,1);//semB
        semctl(semid,2,SETVAL,0);//semC
        semctl(semid,3,SETVAL,0);//semX
    }else
        printf("error semget\n");
    exit(0);
}
