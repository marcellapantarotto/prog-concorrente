#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE  2 //TAMANHO DO BUFFER DE OPERAÇÃO
#define PRODUTORES   1 //PRODUTORES DE CONTEÚDO
#define CONSUMIDORES 1 //CONSUMIDORES DE CONTEÚDO

pthread_mutex_t ACESS_2BF = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t CS = PTHREAD_COND_INITIALIZER;
pthread_cond_t PD = PTHREAD_COND_INITIALIZER;

int buffer_used = 0;

void *consumidor(void *id);
void *produtor  (void *id);

int main(int argc, char * argv[])
{
    pthread_t produtores  [PRODUTORES  ];
    pthread_t consumidores[CONSUMIDORES];
    int *id;
    int i = 0;

    for(i = 0; i < CONSUMIDORES; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
        if(pthread_create(&consumidores[i], NULL, &consumidor, (void*)id)){
            printf("Não pode criar a thread %d\n", i);
            return -1;
        }
    }
    
    for(i = 0; i < PRODUTORES; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
        if(pthread_create(&produtores[i], NULL, &produtor, (void*)id)){
            printf("Não pode criar a thread %d\n", i);
            return -1;
        }
    }

    pthread_join(produtores[0], NULL);
    return 0;
}


void * consumidor(void *id){
    int idc = *((int *)id);
    while(1){
        pthread_mutex_lock(&ACESS_2BF);
            while(buffer_used == 0){      // Espera produtores produzirem algum conteúdo para ser consumido
                printf("BUFFER ESTÁ VAZIO, PRECISO DE PRODUTORES\n");
                pthread_cond_wait(&CS,&ACESS_2BF);
            }
            printf("Consumidor %d consumiu um conteúdo\n", idc);  
            buffer_used--;  // Sinaliza que um espaço do buffer foi ocupado
            printf("Buffer alterado, ocupado %d\n", buffer_used);
            if(buffer_used < BUFFER_SIZE){     // Avisa aos produtores que o buffer não está mais cheio
                pthread_cond_broadcast(&PD);
            }
        pthread_mutex_unlock(&ACESS_2BF);
        sleep(rand()%(idc+5));  
    }
    pthread_exit(0);
}


void * produtor(void *id){
    int idp = *((int *)id);
    while(1){
        printf("Produtor   %d está produzindo um conteúdo\n", idp);
        sleep(rand()%(idp+1));
        pthread_mutex_lock(&ACESS_2BF);
            while(buffer_used == BUFFER_SIZE){      // Espera consumidores consumirem algum conteúdo para poder produzir
                printf("BUFFER ESTÁ CHEIO, PRECISO DE PRODUTORES\n");
                pthread_cond_wait(&PD,&ACESS_2BF);
            }
            printf("Produtor   %d produziu um conteúdo\n", idp);
            buffer_used++;  // Sinaliza que um espaço do buffer foi ocupado
            printf("Buffer alterado, ocupado %d\n", buffer_used);
            if(buffer_used > 1){        // Avisa aos consumidores que o buffer não está mais vazio
                pthread_cond_broadcast(&CS);
            }
        pthread_mutex_unlock(&ACESS_2BF);
        sleep(rand()%(idp+5));
    }
    pthread_exit(0);
}