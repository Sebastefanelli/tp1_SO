# include <stdio.h>
# include <stdlib.h>
#include <unistd.h>
# include <sys/sem.h>

void p(int,int);
void v(int,int);

int main (void){
    int semid= semget(0xa,0,0);
    while(1){
        p(semid,2);//semC
        p(semid,3);//semX
        printf("SC de C\n");
        sleep(1);
        v(semid,0);//semA
        v(semid,1);//semB  
    }
    exit(0);
}
