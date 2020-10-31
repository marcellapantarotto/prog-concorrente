#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define NUMCARROS 2
#define NUMEQUIPES 5
#define CAPACIDADEPISTA 4   // para não terem 2 carros da msm equipe na pista

int qnt = 0;

pthread_t car[NUMCARROS*NUMEQUIPES];    // thread com a quantidade total de carros de todas as equipes
sem_t equipes[NUMEQUIPES];      // semáforo - 1 para cada equipe: como é um vetor de tamanho igual ao número de equipes, apenas 1 carro da equipe pode entrar por vez
sem_t pista;        // semáforo dá região crítica
pthread_mutex_t l = PTHREAD_MUTEX_INITIALIZER;

void * piloto(void *arg);

int main(int argc, char **argv){
    int i;

    int *id;
    srand48(time(NULL));
    // inicialização do semáforo (orbrigatório) --> tem que ser sempre antes da criação das threads, pq quando elas são criadas, já começam a usar o semáforo
    sem_init(&pista,0,CAPACIDADEPISTA);     // Parâmetros: 1) é o semáforo que será inicializado, 2) indicar se S é compartilhado entre threads(0) ou processos(1), 3) quantos threads ou processos compartilham
    for(i=0; i < NUMEQUIPES; i++){ 
        sem_init(&equipes[i],0,1);          // inicializando semáforo de cada equipe, com uma única permissão, pra não deixar entrar 2 carros da msm equipe
    }
    for(i = 0; i < NUMCARROS*NUMEQUIPES;i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
	    pthread_create(&(car[i]),NULL,piloto, (void*) (id));    // criação dos carros/pilotos
    }
   pthread_join(car[0],NULL);
  
}

void * piloto(void *arg){
    int id = *((int *) arg);    // id do carro
    int eq = id%NUMEQUIPES;     // identificador de equipe - espalhar os carros entre as equipes, 2 para cada equipe
    printf("Carro %d da equipe %d criado\n",id,eq);
    
    while(1){
		sleep(drand48()*3);

        // DOWN =
		sem_wait(&equipes[eq]); // controle com companheiro: pega permissão no semáforo da equipe, pra ver se ele é o primeiro ou se outro da equipe já pegou
		    sem_wait(&pista);   // controle da pista
                
                pthread_mutex_lock(&l);
		            qnt++;
                    printf("Carro %d treinando, quantidade: %d\n",id,qnt);
                pthread_mutex_unlock(&l);
                
                sleep(10*id);
                printf("Carro %d saindo da pista\n",id);
                
                pthread_mutex_lock(&l);
                   qnt--;
                pthread_mutex_unlock(&l); 
        // UP = incrementa
		    sem_post(&pista);
        sem_post(&equipes[eq]);
    }
}