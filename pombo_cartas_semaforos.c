// Marcella Pantarotto (13/0143880)

/*
Um pombo correio leva cartas entre os sites A e B, mas só quando o número de cartas acumuladas em sua mochila chegar a 20.
Inicialmente, o pombo fica em A, esperando que existam 20 cartas para carregar, e dormindo enquanto não houver.
Quando as cartas chegam a 20, o pombo deve levar todas as 20 cartas de A para B, e em seguida voltar para A, repetindo este ciclo.
As cartas são escritas por uma quantidade qualquer de usuários. Cada usuário, quando tem uma carta pronta, coloca sua carta na mochila
do pombo e volta a escrever uma nova carta. Caso o pombo tenha partido, ele deve esperar o seu retorno para colar a carta na mochila.
*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define N 10 //número de usuários
#define CARTAS 20 //quantidade de cartas na mochila

int qnt_cartas = 0; // contador de cartas

sem_t sem_pombo;    // o 20o usuário dá um push no pombo pra ele fazer a viagem, 21o usuário fica dormindo, esperando o pombo voltar
sem_t sem_cartas;   // contador de cartas
sem_t sem_mochila;  // sinalização que mochila está cheia

void * f_usuario(void *arg);
void * f_pombo(void *arg);

int main(int argc, char **argv){
    int i;
    int *id;

    sem_init(&sem_pombo, 0, 1); // inicializando semáforo do pombo com 1 permissão, pq tem 1 pombo
	sem_init(&sem_mochila, 0, 0); // inicializando semáforo da mochila vazio, quando num permissões == CARTAS, pombo pode voar
	sem_init(&sem_cartas, 0, CARTAS);

    pthread_t usuario[N];
    for(i = 0; i < N; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
	pthread_create(&(usuario[i]),NULL,f_usuario,  (void *) (id));
    }

    pthread_t pombo;
    id = (int *) malloc(sizeof(int));
    *id = 0;
    pthread_create(&(pombo),NULL,f_pombo, (void*) (id));

    pthread_join(pombo,NULL);
   
}


void * f_pombo(void *arg){
    int value;

    while(1){
        sem_wait(&sem_mochila); // pombo pega a permissão da mochila para levar as cartas
        sem_wait(&sem_pombo);   // pombo pega permissão para voar
        printf("Mochila está cheia e pombo está viajando!\n");
        sleep(1); // tempo de vôo

        printf("Pombo chegou ao local B e irá esvaziar a mochila!\n");
        for (int i = 0; i < CARTAS; i++) {
            sem_post(&sem_cartas); // devolve ao semáforo as permissões das cartas
            qnt_cartas--; // decrementa contador de cartas
        }
        sleep(1); // tempo de vôo

        printf("Pombo voltou para o local A!\n");
        sem_post(&sem_pombo); // devolve permissão para  voar

    }
    pthread_exit(0);
}

void * f_usuario(void *arg){
    int id = *((int *) arg); // id do usuário

    while(1){
        // printf("Usuário %d irá de escrever uma carta\n", id);
	    sem_wait(&sem_cartas); // pega permissão de 1 carta
        sem_wait(&sem_pombo);  // pega a permissão do pombo, para que ele não possa voar
        qnt_cartas++; // incrementa contador de cartas

        printf("Usuário %d acabou de escrever uma carta\t\t Qnt de cartas: %d\n", id, qnt_cartas);
        sleep(1);

        if (qnt_cartas == CARTAS) sem_post(&sem_mochila); // devolve permissão da mochila para pombo poder levar as cartas
        sem_post(&sem_pombo); // devolve permissão do pombo para que ele possa voar
    }
    pthread_exit(0);
}