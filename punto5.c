#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>

int N;  // Número de iteraciones completas

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number_of_iterations>\n", argv[0]);
        return 1;
    }

    N = atoi(argv[1]);

    sem_t *semA = sem_open("/semA", O_CREAT | O_EXCL, 0644, 1);
    sem_t *semB = sem_open("/semB", O_CREAT | O_EXCL, 0644, 0);
    sem_t *semC = sem_open("/semC", O_CREAT | O_EXCL, 0644, 0);

    if (semA == SEM_FAILED || semB == SEM_FAILED || semC == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    pid_t pidB, pidC;

    if ((pidB = fork()) == 0) {  // HijoB
        for (int i = 0; i < N; i++) {
            sem_wait(semB);
            printf("HijoB\n");
            sem_post(semA);
        }
        exit(0);
    } else if (pidB > 0) {
        if ((pidC = fork()) == 0) {  // HijoC
            for (int i = 0; i < N; i++) {
                sem_wait(semC);
                printf("HijoC\n");
                sem_post(semA);
            }
            exit(0);
        } else if (pidC > 0) {  // PadreA
            for (int i = 0; i < N; i++) {
                sem_wait(semA);
                printf("PadreA\n");
                sem_post(semB);
                sem_wait(semA);
                printf("PadreA\n");
                sem_post(semC);
            }
            wait(NULL);
            wait(NULL);
        }
    }

    sem_unlink("/semA");
    sem_unlink("/semB");
    sem_unlink("/semC");
    sem_close(semA);
    sem_close(semB);
    sem_close(semC);

    return 0;
}

