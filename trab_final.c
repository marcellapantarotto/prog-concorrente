// Marcella Pantarotto (13/014880)

/*
Trabalho Final: Fazenda de vacas leiteiras e fábrica de queijos.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define NUMVACAS 10
#define NUMBEZERROS 5
#define NUMFUNCIONARIOS 5
#define MAXLEITE 50
// #define LEITEQUEIJO 10
#define N 10

pthread_mutex_t mutex_leite = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t vaca_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t bezerro_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t funcionario_cond = PTHREAD_COND_INITIALIZER;

sem_t grupo[N];

void *vaca(void *id);
void *bezerro(void *id);
void *funcionario(void *id);

int leite = 0;  // contador
int vQuer = 0;      // vaca quer
int bQuer = 0;      // bezerro quer

int main(int argc, char **argv) {
    
    int erro;
    int i, n, m;
    int *id;

    pthread_t vid[NUMVACAS];
    for (i = 0; i < NUMVACAS; i++)  {
        id = (int *) malloc(sizeof(int));
        *id = i;
        erro = pthread_create(&vid[i], NULL, vaca, (void *) (id));

        if(erro){
            printf("erro na criacao do thread %d\n", i);
            exit(1);
        }
    }

    pthread_t bid[NUMBEZERROS];
    for (i = 0; i < NUMBEZERROS; i++)  {
        id = (int *) malloc(sizeof(int));
        *id = i;
        erro = pthread_create(&bid[i], NULL, bezerro, (void *) (id));

        if(erro){
            printf("erro na criacao do thread %d\n", i);
            exit(1);
        }
    }

    pthread_t fid[NUMVACAS];
    for (i = 0; i < NUMVACAS; i++)  {
        id = (int *) malloc(sizeof(int));
        *id = i;
        erro = pthread_create(&fid[i], NULL, funcionario, (void *) (id));

        if(erro){
            printf("erro na criacao do thread %d\n", i);
            exit(1);
        }
    }

    pthread_join(vid[0],NULL);
}

void *vaca (void *arg){
    int id = *((int *) arg);    // id da vaca

    while(1){
    sleep(rand()%(id+1)+2);
    pthread_mutex_lock(&mutex_leite);
        vQuer = vQuer;
        while (leite > 0) {    // enquanto tiver leite
            pthread_cond_wait(&vaca_cond, &mutex_leite);   // adormece vaca
        }
        vQuer--;
    pthread_mutex_unlock(&mutex_leite);
        
        leite = leite + 10;
        printf("\nVaca %d vai pastar. Quantidade de leite: %d\n", id, leite);
        sleep(2);

    pthread_mutex_lock(&mutex_leite);
        // acordar bezerro e funcionario
        if (leite == MAXLEITE){
            printf("Banco de leite cheio!\n\n");
            pthread_cond_signal(&bezerro_cond);
            pthread_cond_signal(&funcionario_cond);
        }
    pthread_mutex_unlock(&mutex_leite);
    }
    pthread_exit(0);

}

void *bezerro(void *arg) {
    int id = *((int *) arg);    // id do bezerro
    while(1) {
        sleep(5);
        pthread_mutex_lock(&mutex_leite);     // canibal pega o lock
            bQuer++;
            while (leite == 0 || vQuer > 0) {         // se não tiver comida
                printf("Acabou o leite! Acordem vacas!\n");
                pthread_cond_signal(&vaca_cond);    // acorda primeiro o cozinheiro para não acontecer que todos os canibais durmam e não ter ninguém para acordá-lo
                pthread_cond_wait(&bezerro_cond, &mutex_leite); // adormece canibal
            }
            bQuer--;
            leite = leite - 5;    //pegar uma porção de comida
            printf("Bezerro %d está mamando. Quantidade de leite: %d\n", id, leite);
        pthread_mutex_unlock(&mutex_leite);   // canibal solta o lock
    }
    sleep(5);
}

void *funcionario(void *arg) {
    int id = *((int *) arg);    // id do funcionario

    while(1){
      sleep(2);
      pthread_mutex_lock(&mutex_leite);
        while(leite == 0 || vQuer != 0 || bQuer != 0) {
                pthread_cond_wait(&funcionario_cond, &mutex_leite);
            }
        leite = leite - 5;    //pegar uma porção de comida
        printf("Funcionario %d está ordenhando uma vaca. Quantidade de leite: %d\n", id, leite);
      pthread_mutex_unlock(&mutex_leite);
    }
    sleep(2);
}