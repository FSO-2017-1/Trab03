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
int BUFFER_ITERATOR = 0;
char NAME_FILE[20];
int NUMBER_THREADS=4; 
int producer_buffer[BUFFER_SIZE];  

pthread_mutex_t lock, count_lock;

void set_end_flag(int sig){
  TERMINATE_FLAG = 1;
}

void file_insert(const char* message, FILE* file){
  //Verificar qual é o modo de abertura correto, não é a, nem w
  pthread_mutex_lock(&lock);
  file = fopen(NAME_FILE,"a");
  if (file == NULL) {
    printf("Error ao abrir arquivo");
  }
  fprintf (file, "%s\n", message);
  fclose(file);
  pthread_mutex_unlock(&lock);
}

int terminate_program(){}

void increment_iterator(){
  pthread_mutex_lock(&count_lock);
  BUFFER_ITERATOR++;
  pthread_mutex_unlock(&count_lock);
}

void decrement_iterator(){
  pthread_mutex_lock(&count_lock);
  BUFFER_ITERATOR--;
  pthread_mutex_unlock(&count_lock);
}

int producer_thread(void *args){

  struct info_thread *info = (struct info_thread *) args;

  while(TERMINATE_FLAG == 0){
    while(BUFFER_ITERATOR<BUFFER_SIZE){
    // esta gerando apenas numeros positvos,arrumar   
    int producer_random_number = rand();

    increment_iterator();
    producer_buffer[BUFFER_ITERATOR] = producer_random_number;
    
    printf("\nThread produtora\n");
    printf(" i= %d random number = %d\n", BUFFER_ITERATOR, producer_random_number);

    char message[20];
    sprintf(message, "[producao]: Numero gerado: %d", producer_random_number);

    file_insert(message, info->file);
    
    // arrumar o tempo gerado
    sleep(1);

    }
  }
}

// Lembrar que por ser um problema de producer/consumer, quando um buffer é consumido
// ele deve ser decrementado.

int consumer_thread(void *args){
  struct info_thread *info = (struct info_thread *) args;
  
  while(TERMINATE_FLAG == 0){
    
    // definir melhor esse while
      while(BUFFER_ITERATOR >= 0){      
        if(BUFFER_ITERATOR > 0){
          int number = BUFFER_ITERATOR;
          printf("Thread consumidora %s\n", info->type_thread);
          printf("i=%d numero lido %d\n", BUFFER_ITERATOR, producer_buffer[number]);

          char message[20];
          sprintf(message, "[consumo %s]: Numero lido: %d", info->type_thread, producer_buffer[number]);
          file_insert(message, info->file);

          decrement_iterator();

          // arrumar o tempo
          sleep(2);
        }else{
          // nothing to do
        }
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
  info_thread[1].number = 0;
  info_thread[1].file = file;
  pthread_create(&thread[1], NULL, &producer_thread, &info_thread[1]);

  //Cria as thread consumidoras
  info_thread[2].type_thread = "consumo a";
  pthread_create(&thread[2], NULL, &consumer_thread, &info_thread[2]);

  //info_thread[3].type_thread = "consumo b";
  //pthread_create(&thread[3], NULL, &consumer_thread, &info_thread[3]);


  while(!TERMINATE_FLAG){
    signal(SIGINT, set_end_flag);
  }

  if(TERMINATE_FLAG){
    printf("\n[aviso]: Termino Solicitado. Aguardando threads...\n");
    for (i = 0; i < 3; i++){
       pthread_join(thread[i], NULL);
    }
    printf("[aviso]: Aplicacao encerrada\n");
    return 0;
  }

  return 0;
}
