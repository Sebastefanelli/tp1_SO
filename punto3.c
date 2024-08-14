#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

void *hiloa(void *p);
void *hilob(void *p);
void *hiloc(void *p);

pthread_mutex_t ma= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mb=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mc=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mx=PTHREAD_MUTEX_INITIALIZER;

int total;
int i;
bool termino = false;

int main(int argc, char * argv[]){

  total=atoi(argv[1]);
  printf("total=%d\n",total);

  pthread_t hiloA, hiloB, hiloC;

  pthread_mutex_lock(&mc);
  pthread_mutex_lock(&mx);

  pthread_create(&hiloA, NULL, hiloa, NULL);
  pthread_create(&hiloB, NULL, hilob, NULL);
  pthread_create(&hiloC, NULL, hiloc,NULL);

  pthread_join(hiloA, NULL);
  pthread_join(hiloB, NULL);
  pthread_join(hiloC,NULL);

  return 0;
}
void *hiloa(void *p) {
  while(!termino){
    pthread_mutex_lock(&ma);
      if(termino) break;
      printf("hola soy el hilo A\n");
    pthread_mutex_unlock(&mx);
  }
  pthread_mutex_unlock(&mx);
}

void *hilob(void *p) {
  while(!termino){
    pthread_mutex_lock(&mb);
    pthread_mutex_lock(&mx);
      if(termino) break;
      printf("hola soy el hilo B\n");
    pthread_mutex_unlock(&ma);
    pthread_mutex_unlock(&mc);
  }
  pthread_mutex_unlock(&ma);
  pthread_mutex_unlock(&mc);
}

void *hiloc(void*p){
  while(!termino){
    pthread_mutex_lock(&mc);
    pthread_mutex_lock(&mx);
      i++;
      printf("hola soy el hilo C\n");
      printf("i=%d\n",i);
    if(i==total){
      termino=true;
      break;
    }
    pthread_mutex_unlock(&ma);
    pthread_mutex_unlock(&mb);
  }
  pthread_mutex_unlock(&ma);
  pthread_mutex_unlock(&mb);
}
