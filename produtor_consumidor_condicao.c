/*
seguir pseudo código dos slides
pensar em 1 produtor e 1 consumidor só (uma thread para cada)
sleep = cond_wait ; wakeup = signal
terá 2 condições: uma para produtores pararem e outra para consumidores pararem
produtor dá signal na condição do consumidor e o consumidor dá signal na condição do produtor

buffer --> vetor de tamanho N
com 2 índices (contadores): 1 para o produtor inserir o próximo ítem e 1 para marcar onde o consumidor irá retirar o próximo ítem
índices são acessados dentro da região de lock
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define PR 1 //número de produtores
#define CN 1 // número de consumidores
#define N 5  //tamanho do buffer

void *produtor(void *meuid);
void *consumidor(void *meuid);
void print_buffer();
int produce_item();
void insert_data(int data, int i);
void remove_data(long int i);
int find_index();

// declarando e inicializando o vetor do buffer com 0 em todas as posições
int buffer[N] = {0};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t produtor_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t consumidor_cond = PTHREAD_COND_INITIALIZER;

int count = 0;
int index_insert = 0;
int index_remove = 0;

void __attribute__ ((destructor))  dtor() { print_buffer(); };

void main(argc, argv)
int argc;
char *argv[];
{

  int erro;
  int i, n, m;
  int *id;

  print_buffer();

  pthread_t tPid[PR];

  for (i = 0; i < PR; i++)
  {
    id = (int *)malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tPid[i], NULL, produtor, (void *)(id));

    if (erro) {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  pthread_t tCid[CN];

  for (i = 0; i < CN; i++)
  {
    id = (int *)malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tCid[i], NULL, consumidor, (void *)(id));

    if (erro) {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  pthread_join(tPid[0], NULL);  
}

void *produtor(void *pi) {
  int item;

  while (1) {
    sleep(rand() % 2);
    item = produce_item();

    pthread_mutex_lock(&mutex);
      while (count == N) {
        pthread_cond_wait(&produtor_cond, &mutex);
      }
      index_insert = find_index();
      insert_data(item, index_insert);
      count += 1;
    pthread_mutex_unlock(&mutex);

    printf("PRODUTOR está produzindo conteúdo\n\n");
    print_buffer();
    sleep(1);
    

    if (count == 1){
      // como só tem 1 consumidor, não há problema usar signal
    pthread_cond_signal(&consumidor_cond);
    } 
  }
  pthread_exit(0);
}

void *consumidor(void *pi) {
  int item;

  while (1) {
    sleep(rand() % 2);

    pthread_mutex_lock(&mutex);
      while (count == 0) {
        pthread_cond_wait(&consumidor_cond, &mutex);
      }
      index_remove = find_index();
      // printf(index_remove);
      remove_data(index_remove);
      count -= 1;
    pthread_mutex_unlock(&mutex);

    printf("CONSUMIDOR está consumindo conteúdo\n\n");
    print_buffer();
    sleep(1);
    

    if (count == N - 1){
      // como só tem 1 produtor, não há problema usar signal
    pthread_cond_signal(&produtor_cond);
    } 
  }
  pthread_exit(0);
}

void print_buffer() {
  printf("BUFFER:\n");
  for (size_t i = 0; i < N; i++) {
    printf("buffer[%ld] = %d\n", i, buffer[i]);
  }
  printf("\n");
}

int produce_item(){
  return 1;
}

void insert_data(int data, int index){
  // TODO
  printf("--Inserindo data no buffer\n");
  buffer[index] = data;
  // print_buffer();
}

void remove_data(long int index){
  printf("--Removendo data do buffer\n");
  buffer[index] = 0;
  // print_buffer();
}

int find_index(){
  int aux = -1;
  for (int i = 0; i < N; i++) {
    if (buffer[i] == 0) {
      aux += 1;
    }
    // printf("~index = %d\n", i);
    // printf("aux = %d\n", aux);
    return i;
  }
}