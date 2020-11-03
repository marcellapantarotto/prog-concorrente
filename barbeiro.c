// Marcella Pantarotto (13/0143880)

/*
 * Problema do barbeiro dorminhoco.
 * Um outro problema clássico de IPC acontece em uma barbearia. Na barbearia há um barbeiro, uma cadeira de barbeiro e N cadeiras para eventuais 
 * clientes esperarem a vez. Quando não há clientes, o barbeiro senta-se e cai no sono (fica bloqueado). Quando chega um cliente, ele precisa
 * acordar o barbeiro. Se outros clientes chegarem enquanto o barbeiro estiver cortando o cabelo de um cliente, eles se sentarão (se houver cadeiras
 * vazias) ou sairão da barbearia (se todas as cadeiras estiverem ocupadas). O problema é programar o barbeiro e os M clientes sem cair em condições
 * de disputa. Esse problema é semelhante a situações com várias filas, como uma mesa de atendimento de telemarketing com diversos atendentes e com
 * um sistema computadorizado de chamadas em espera, atendendo a um número limitado de chamadas que chegam.
 */ 

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N_CLIENTES 50
#define N_CADEIRAS 5

sem_t sem_cadeiras_espera;  // semáforo das cadeiras da fila de espera
sem_t sem_barbeiro;         // semáforo do barbeiro
sem_t sem_corte_feito;      // semáforo dos clientes que já cortaram o cabelo
pthread_mutex_t mux_cadeira_corte = PTHREAD_MUTEX_INITIALIZER;    // lock da cadeira do barbeiro, onde o cliente se senta para ter o seu cabelo cortado

void * f_barbeiro(void *v) {

  while(1) {
    //...Esperar/dormindo algum cliente sentar na cadeira do barbeiro (e acordar o barbeiro) - semaforo B, 0 permissoes
    sem_wait(&sem_barbeiro);
    sleep(1); //Cortar o cabelo do cliente 
    printf("Barbeiro cortou o cabelo de um cliente\n"); 
    //...Liberar/desbloquear o cliente
    sem_post(&sem_corte_feito);
  }
  pthread_exit(0);
}

void* f_cliente(void* v) {
  int id = *(int*) v;
  sleep(id%5);
  if(sem_trywait(&sem_cadeiras_espera) == 0){ //conseguiu pegar uma cadeira de espera
    printf("Cliente %d entrou na barbearia \n", id);
    //... pegar/sentar a cadeira do barbeiro - semaforo
    pthread_mutex_lock(&mux_cadeira_corte);
    printf("Cliente %d sentou na cadeira do barbeiro\n", id);
    //... liberar a sua cadeira de espera- post nas cadeiras
    sem_post(&sem_cadeiras_espera);
    //... acordar o barbeiro para cortar seu cabelo - post no semaforo B do barbeiro
    sem_post(&sem_barbeiro);
    //... aguardar o corte do seu cabelo
    sem_wait(&sem_corte_feito);
    //... liberar a cadeira do barbeiro - lock (só tem 1 cadeiro)
    pthread_mutex_unlock(&mux_cadeira_corte);
    sleep(3);
    printf("Cliente %d cortou o cabelo e foi embora \n",id);

  }else{//barbearia cheia
	printf("Barbearia cheia, cliente %d indo embora\n",id);
  }

  pthread_exit(0);
}

int main() {
  pthread_t thr_clientes[N_CLIENTES], thr_barbeiro;
  int i, id[N_CLIENTES];

  sem_init(&sem_cadeiras_espera, 0, N_CADEIRAS);
  sem_init(&sem_barbeiro, 0, 0);
  sem_init(&sem_corte_feito, 0, 0);
  
  for (i = 0; i < N_CLIENTES; i++) {
    id[i] = i;
    pthread_create(&thr_clientes[i], NULL, f_cliente, (void*) &id[i]);
  }

  pthread_create(&thr_barbeiro, NULL, f_barbeiro, NULL);
  
  for (i = 0; i < N_CLIENTES; i++) 
    pthread_join(thr_clientes[i], NULL);

  /* Barbeiro assassinado */
  
  return 0;
}
