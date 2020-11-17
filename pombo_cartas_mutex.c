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
int qnt_cartas = 0;

pthread_cond_t usuario_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t pombo_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mochila = PTHREAD_MUTEX_INITIALIZER;

void * f_usuario(void *arg);
void * f_pombo(void *arg);

int main(int argc, char **argv){
    int i, erro;
    int *id;

    pthread_t usuario[N];
    for(i = 0; i < N; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
	    erro = pthread_create(&(usuario[i]),NULL,f_usuario,  (void *) (id));

        if(erro) {
            printf("ERRO na criacao da thread %d\n", i);
            exit(1);
        }
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
        
        pthread_mutex_lock(&mochila);
            while (qnt_cartas < CARTAS) {
                pthread_cond_wait(&pombo_cond, &mochila);
                printf("Mochila ainda não está cheia, pombo não pode sair\n");
            }
            
            printf("Mochila está cheia! Pombo irá viajar!\n");            
            local = B;
            sleep(2+rand()%2);

            printf("Pombo chegou ao local B e irá esvaziar a mochila!\n");
            qnt_cartas = 0;
            sleep(2);
            
            printf("Pombo voltou para o local A!\n");
            local = A;
            sleep(3);

            if (local == A)
                pthread_cond_broadcast(&usuario_cond);

        pthread_mutex_unlock(&mochila);
    }
}

void * f_usuario(void *arg){
    int id = *((int *) arg);    // id do usuário

    while(1){
	    //Escreve uma carta
        //Caso o pombo não esteja em A ou a mochila estiver cheia, então dorme	
        //Posta sua carta na mochila do pombo
        //Caso a mochila fique cheia, acorda o ṕombo

        
        pthread_mutex_lock(&mochila);
        // printf("\tid = %d\n", id);
            while (local != A || qnt_cartas == CARTAS) {
                pthread_cond_signal(&pombo_cond);
                pthread_cond_wait(&usuario_cond, &mochila);
                
                if (local != A)
                    printf("Pombo não estão no local A!\tUsuário %d não pode escrever\n", id);
                if (qnt_cartas == CARTAS)
                    printf("Mochila do pombo está cheia!\tUsuário %d não pode escrever\n", id);
            }

            qnt_cartas++;
        pthread_mutex_unlock(&mochila);
       
        printf("Usuário %d acabou de escrever uma carta\t\t Qnt de cartas: %d\n", id, qnt_cartas);
        sleep(2);
        
    }
}