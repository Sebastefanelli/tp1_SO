# include <stdio.h>
# include <stdlib.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/sem.h>

int main(void){
    int semid= semget(0xa,0,0);
    if (semid!=-1){
        printf("semA = %d \n", semctl(semid,0,GETVAL));
        printf("semB = %d \n", semctl(semid,1,GETVAL));
        printf("semC = %d \n", semctl(semid,2,GETVAL));
        printf("semX = %d \n", semctl(semid,3,GETVAL));
    }else
        printf("error semget");
    exit(0);
}
