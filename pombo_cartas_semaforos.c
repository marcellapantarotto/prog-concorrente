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
#define A 0
#define B 1

#define CARTAS 20 //quantidade de cartas na mochila

int local = 0;

sem_t sem_cartas;
sem_t sem_pombo;    // o 20o usuário dá um push no pombo pra ele fazer a viagem
// 21o usuário fica dormindo, esperando o pombo voltar

void * f_usuario(void *arg);
void * f_pombo(void *arg);

int main(int argc, char **argv){
    int i;

    pthread_t usuario[N];
    int *id;
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
  
    while(1){
        //Inicialmente está em A, aguardar/dorme a mochila ficar cheia (20 cartas)
        //Leva as cartas para B e volta para A
        //Acordar os usuários   
    }
}

void * f_usuario(void *arg){
    int id = *((int *) arg);    // id do usuário

    while(1){
	    //Escreve uma carta
        //Caso o pombo não esteja em A ou a mochila estiver cheia, então dorme	
        //Posta sua carta na mochila do pombo
        //Caso a mochila fique cheia, acorda o ṕombo
    }
}