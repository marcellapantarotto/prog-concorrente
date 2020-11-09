#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#define N 5					// quantidade de talheres e filósofos
#define ESQ(id) (id)		// talher da esquerda
#define DIR(id)	(id+1)%N	// talher da direita

sem_t s[N];		// semáforo para os telheres

void *filosofos (void *arg);

void pega_talher (int n);		// funções executadas pelos filósofo
void devolve_talher (int n);	// funções executadas pelos filósofo

int main () {
	int i;
	int *id;
	//semaforo 
	for (i=0; i<N; i++) {
		sem_init(&s[i], 0, 1);		// inicialização semáforo; apenas 1 permissão pq cada um acessa 1 talher
	}
	pthread_t r[N]; 

	//criacao das threads de filosofos
       for (i = 0; i < N ; i++) {
			id = (int *) malloc(sizeof(int));
			*id = i;
			pthread_create(&r[i], NULL, filosofos, (void*) (id));
	}

        pthread_join(r[0],NULL);
	return 0;
}


void *filosofos (void *arg) {
	int n = *((int *) arg);
	while(1) {
		//pensar
		printf("Filosofo %d pensando ...\n", n);
		sleep(3);
		
		pega_talher(n);
		//comer
		printf("\tFilosofo %d comendo ...\n", n);
		sleep(3);
        
		printf("\tFilosofo %d acabou de comer ...\n", n);
		devolve_talher(n);
	} 
}

void pega_talher (int n) {		// n = id do filófoso
	// if(n == 4){
	if(n/2 == 0){
		sem_wait(&s[DIR(n)]);
		sleep(1);
		sem_wait(&s[ESQ(n)]);
	}else{
		sem_wait(&s[ESQ(n)]);
		sleep(1);
		sem_wait(&s[DIR(n)]);
	} 
}

void devolve_talher (int n) {
	sem_post(&s[ESQ(n)]);
	sem_post(&s[DIR(n)]);
}