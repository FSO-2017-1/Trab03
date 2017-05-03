#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

int TERMINATE_FLAG = 0;

void set_end_flag(int sig){
  TERMINATE_FLAG = 1;
}

struct info_thread_t{
  int number;
  int qtd_lines;
};

void *print(void *args){
  struct info_thread_t *info = (struct info_thread_t *) args;
  while(!TERMINATE_FLAG){
    // Escrita de arquivo
    printf("doidão");
    usleep(150);
  }
}


int main(int argc, char const *argv[]) {

  char *name_file;

  name_file = argv[1];
  name_file =  strcat(name_file,".txt");
  FILE* file;

  file = fopen(name_file,"wb");
  if (file == NULL) {
    printf("Error ao abrir arquivo");
    exit(1);
  }
  printf("%s\n",name_file);

  pthread_t thread[4];
  struct info_thread_t info_thread[4];

  //Cria 4 threads
  for (int i = 0; i < 4; i++){
    printf("%d\n",i);
    pthread_create(&thread[i],NULL,&print,&info_thread[i]);
  }

  while(!TERMINATE_FLAG){
    signal(SIGINT, set_end_flag);
  }

if(TERMINATE_FLAG){
  printf("\nEncerrando a aplicação. Aguardando finalização de threads....\n");
  for (int i = 0; i < 4; i++){
     pthread_join(thread[i], NULL);
  }
  printf("Aplicação encerrada com sucesso!\n"
      "Informações: \n");
  for (int i = 0; i < 4; i++){
    printf("Thread :  linhas\n");
  }
  return 0;
}

 fclose(file);
  return 0;
}
