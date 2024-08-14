# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/sem.h>

void p(int semid, int sem)//wait
{
    struct sembuf buf;
    buf.sem_num=sem;
    buf.sem_op=-1;
    buf.sem_flg=0;
    semop(semid,&buf,1);
}


void v(int semid, int sem)//signal
{
    struct sembuf buf;
    buf.sem_num=sem;
    buf.sem_op=1;
    buf.sem_flg=0;
    semop(semid,&buf,1);
}
