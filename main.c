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
int buffer_tam_max=0;

pthread_mutex_t lock, count_lock, decount_lock, consumer_lock;

void set_end_flag(int sig){
  TERMINATE_FLAG = 1;
}

void file_insert_smallest(const int message, FILE* file){
  //Verificar qual é o modo de abertura correto, não é a, nem w
  pthread_mutex_lock(&lock);
  file = fopen(NAME_FILE,"a");
  if (file == NULL) {
    printf("Error ao abrir arquivo");
  }
  fprintf (file, "\n[aviso]: Menor numero gerado: %d", message);
  fclose(file);
  pthread_mutex_unlock(&lock);
}

void file_insert_biggest(const int message, FILE* file){
  //Verificar qual é o modo de abertura correto, não é a, nem w
  pthread_mutex_lock(&lock);
  file = fopen(NAME_FILE,"a");
  if (file == NULL) {
    printf("Error ao abrir arquivo");
  }
  fprintf (file, "\n[aviso]: Maior numero gerado: %d", message);
  fclose(file);
  pthread_mutex_unlock(&lock);
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
  pthread_mutex_lock(&decount_lock);
  BUFFER_ITERATOR--;
  pthread_mutex_unlock(&decount_lock);
}

void add_biggest_value(struct info_thread *thread, int value){
  int actual_value = thread->biggest_value;
  if(value > actual_value){
    thread->biggest_value = value;
  }
}

void add_smallest_value(struct info_thread *thread, int value){
  int actual_value = thread->smallest_value;
  if(value < actual_value){
    thread->smallest_value = value;
  }

}

void producer_thread(struct info_thread *info,void *args){
  // struct info_thread *info = (struct info_thread *) args;
  while(BUFFER_ITERATOR<BUFFER_SIZE){
    while(TERMINATE_FLAG == 0){
      if(buffer_tam_max <= BUFFER_ITERATOR){
        buffer_tam_max++;
      }
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

void consumer_thread(struct info_thread *info,void *args){
  // struct info_thread *info = (struct info_thread *) args;

  //Melhorar essa condição, o programa não está encerrando corretamente.
  while(BUFFER_ITERATOR >=0){
    pthread_mutex_lock(&consumer_lock);
    if(BUFFER_ITERATOR > 0){

      int number = BUFFER_ITERATOR;
      printf("Thread consumidora %s\n", info->type_thread);
      printf("i=%d numero lido %d\n", BUFFER_ITERATOR, producer_buffer[number]);

      char message[20];
      sprintf(message, "[%s]: Numero lido: %d", info->type_thread, producer_buffer[number]);
      file_insert(message, info->file);
      add_biggest_value(info, producer_buffer[number]);
      add_smallest_value(info, producer_buffer[number]);

      printf("\nMAIOR NUMERO DA THREAD %s %d\n", info->type_thread, info->biggest_value);
      printf("MENOR NUMERO DA THREAD %s %d\n", info->type_thread, info->smallest_value);

      decrement_iterator();

    }
    pthread_mutex_unlock(&consumer_lock);
     // arrumar o tempo
    sleep(3);
  }
}


int handler_thread(struct info_thread *info,void *args){

  }





void get_biggest_number(struct info_thread *threadA, struct info_thread *threadB){

  int value;

  if(threadA->biggest_value > threadB->biggest_value){
    value = threadA->biggest_value;
  }else if (threadB->biggest_value > threadA->biggest_value){
    value = threadB->biggest_value;
  }else{
    value = threadA->biggest_value;
  }

  file_insert_biggest(value,threadA->file);
}

void get_smallest_number(struct info_thread *threadA, struct info_thread *threadB){

  int value;

  if(threadA->smallest_value < threadB->smallest_value){
    value =  threadA->smallest_value;
  }else if (threadB->smallest_value < threadA->smallest_value){
    value =  threadB->smallest_value;
  }else{
    value =  threadB->smallest_value;
  }
  file_insert_smallest(value,threadA->file);
}


void end_program(int signal,struct info_thread *info_thread){

  printf("\n[aviso]: Termino Solicitado. Aguardando threads...\n");
  file_insert("\n[aviso]: Termino Solicitado. Aguardando threads...\n", info_thread[1].file);
  get_biggest_number(&info_thread[2], &info_thread[3]);
  get_smallest_number(&info_thread[2], &info_thread[3]);
  printf("[aviso]: Aplicacao encerrada\n");

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
  info_thread[1].number = 1;
  info_thread[1].file = file;
  pthread_create(&thread[1], NULL, &producer_thread, &info_thread[1]);

  //Cria as thread consumidoras
  info_thread[2].type_thread = "consumo a";
  info_thread[2].biggest_value = 0;
  info_thread[2].smallest_value = 99999999999;
  pthread_create(&thread[2], NULL, &consumer_thread, &info_thread[2]);

  info_thread[3].type_thread = "consumo b";
  info_thread[3].biggest_value = 0;
  info_thread[3].smallest_value = 99999999999;
  pthread_create(&thread[3], NULL, &consumer_thread, &info_thread[3]);

  info_thread[0].type_thread = "handler";
  info_thread[0].file = file;
  pthread_create(&thread[0], NULL, &handler_thread, &info_thread[0]);

  while(!TERMINATE_FLAG){
    signal(SIGINT, set_end_flag);
  }

  printf("\n \n mamilinhos %d\n", buffer_tam_max);
  if(TERMINATE_FLAG){
    file_insert("\n[aviso]: Termino Solicitado. Aguardando threads...\n", info_thread[0].file);

    get_biggest_number(&info_thread[2], &info_thread[3]);
    get_smallest_number(&info_thread[2], &info_thread[3]);

    char message[20];
    sprintf(message, "[aviso]: Maior ocupacao de buffer: %d\n", buffer_tam_max);
    file_insert(message, info_thread[0].file);

    pthread_cancel (&info_thread[0]);
    pthread_cancel (&info_thread[1]);
    pthread_cancel (&info_thread[2]);
    pthread_cancel (&info_thread[3]);

    file_insert("\n[aviso]: Aplicacao encerrada.\n", info_thread[0].file);
    signal(SIGINT,end_program);
    return 0;
  }

  return 0;
}
