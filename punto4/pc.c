#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <unistd.h>

void p(int, int);
void v(int, int);

int main(int argc, char *argv[]) {
  int n = atoi(argv[1]);
  int semid = semget(0xa, 0, 0);
  for (int i = 0; i < n; i++) {
    p(semid, 2); // semC
    p(semid, 3); // semX
    printf("SC de C\n");
    sleep(1);
    v(semid, 0); // semA
    v(semid, 1); // semB
  }
  exit(0);
}
