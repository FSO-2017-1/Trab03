#define _DEFAULT_SOURCE
#define BUFFER_SIZE 50
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
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

void file_insert(const char* message, FILE* file){
  //Verificar qual é o modo de abertura correto, não é a, nem w
  file = fopen(NAME_FILE,"a");
  if (file == NULL) {
    printf("Error ao abrir arquivo");
  }
  fprintf (file, "%s\n", message);
  fclose(file);
}

int terminate_program(){}

int producer_thread(void *args){
  struct info_thread *info = (struct info_thread *) args;

  int producer_buffer[BUFFER_SIZE];  
  int i = 0;
  // não esta incrementando o contador
  
    while(!TERMINATE_FLAG){
      for(i=0; i<BUFFER_SIZE; i++){
      // esta gerando apenas numeros positvos,arrumar   
      int producer_random_number = rand();
      producer_buffer[i] = producer_random_number;

      printf("Thread produtora criada");
      printf(" i= %d random number = %d\n", i, producer_random_number);

      char message[20];
      sprintf(message, "[producao]: Numero gerado: %d", producer_random_number);
      //char *message = strcat(producer_random_number, "[producao]: Numero gerado: \n");

      file_insert(message, info->file);
      // arrumar o tempo gerado
      sleep(1);
    }
  }
}

int main(int argc, char const *argv[]) {
  srand(time(NULL));

  int i;
  strcpy(NAME_FILE,argv[1]);
  strcpy(NAME_FILE,strcat(NAME_FILE,".txt"));
  FILE* file;

  pthread_t thread[NUMBER_THREADS];
  struct info_thread info_thread[NUMBER_THREADS];

  //Cria a thread produtora
  info_thread[0].file = file;
  pthread_create(&thread[0], NULL, &producer_thread, &info_thread[0]);

  //Cria as threads
  /*for (i = 0; i < NUMBER_THREADS; i++){
      pthread_create(&thread[i],NULL,&save_log,&info_thread[i]);
  }*/

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
