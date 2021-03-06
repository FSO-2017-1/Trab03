#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include "TypeStruct.h"

int TERMINATE_FLAG = 0;
char NAME_FILE[20];
int NUMBER_THREADS=4;

void set_end_flag(int sig){
  TERMINATE_FLAG = 1;
}

void *save_log(void *args){
  struct info_thread *info = (struct info_thread *) args;


  while(!TERMINATE_FLAG){
    // Escrita de arquivo
    printf("%d\n", info->number);
    usleep(150);
  }
}

void file_insert(){

  strcpy(NAME_FILE,strcat(NAME_FILE,".txt"));
  FILE* file;

  file = fopen(NAME_FILE,"wb");
  if (file == NULL) {
    printf("Error ao abrir arquivo");
  }
  fclose(file);
}

int terminate_program(){}

int main(int argc, char const *argv[]) {


  int i;
  strcpy(NAME_FILE,argv[1]);
  //verificar onde é o melho lugar
  file_insert();

  pthread_t thread[NUMBER_THREADS];


  struct info_thread info_thread[NUMBER_THREADS];

  //Cria as threads
  for (i = 0; i < NUMBER_THREADS; i++){
      pthread_create(&thread[i],NULL,&save_log,&info_thread[i]);
  }

  while(!TERMINATE_FLAG){
    signal(SIGINT, set_end_flag);
  }

  if(TERMINATE_FLAG){
    printf("\n[aviso]: Termino Solicitado. Aguardando threads...\n");
    for (i = 0; i < NUMBER_THREADS; i++){
       pthread_join(thread[i], NULL);
    }
    printf("[aviso]: Aplicacao encerrada\n");
    return 0;
  }

  return 0;
}
