#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MA 10 //macacos que andam de A para B
#define MB 10 //macacos que andam de B para A

// (se usar apenas um lock garante a exclusão mútua da região crítica por apenas 1 leitor ou 1 escritor)
pthread_mutex_t lock_travessia = PTHREAD_MUTEX_INITIALIZER; // lock da travessia
pthread_mutex_t lock_corda = PTHREAD_MUTEX_INITIALIZER;     // lock da corda

// int vez = (rand() % 2);
int vez = 0;

// leitores
void * macacoAB(void * a) {
    int i = *((int *) a);    
    while(1){
    // região de exclusão mútua
    // pthread_mutex_lock(&lock_travessia); // macaco de A->B pega o lock da travessia
    if (vez == 0){
      pthread_mutex_lock(&lock_corda);
        //Procedimentos para acessar a corda
        printf("Macaco %d passado de A para B \n", i);
        vez = (rand() % 2);
        sleep(1);
      pthread_mutex_unlock(&lock_corda);
    }

    //Procedimentos para quando sair da corda
    // pthread_mutex_unlock(&lock_travessia); // macaco de A->B libera o lock da travessia
    }
    pthread_exit(0);
}

// leitores
void * macacoBA(void * a) {
    int i = *((int *) a);    
    while(1){
    // região de exclusão mútua
    // pthread_mutex_lock(&lock_travessia); // macaco de B->A pega o lock da travessia
    if (vez == 1){
      pthread_mutex_lock(&lock_corda);
        //Procedimentos para acessar a corda
        printf("Macaco %d passado de B para A \n", i);
        vez = (rand() % 2);
        sleep(1);
      pthread_mutex_unlock(&lock_corda);
    }
   
    //Procedimentos para quando sair da corda
    // pthread_mutex_unlock(&lock_travessia); // macaco de B->A libera o lock da travessia
    }
    pthread_exit(0);
}

// são como os escritores então vai precisar de 2 locks nos 2 macacos e gorila
void * gorila(void * a){
    while(1){
	//Procedimentos para acessar a corda
	printf("Gorila passado de A para B \n");
	sleep(5);
        //Procedimentos para quando sair da corda
     }
    pthread_exit(0);
}

int main(int argc, char * argv[])
{
    pthread_t macacos[MA+MB];
    int *id;
    int i = 0;

    for(i = 0; i < MA+MB; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
        if(i%2 == 0){
          if(pthread_create(&macacos[i], NULL, &macacoAB, (void*)id)){
            printf("Não pode criar a thread %d\n", i);
            return -1;
          }
        }else{
          if(pthread_create(&macacos[i], NULL, &macacoBA, (void*)id)){
            printf("Não pode criar a thread %d\n", i);
            return -1;
          }
        }
    }
    // pthread_t g;
    // pthread_create(&g, NULL, &gorila, NULL);

  
    pthread_join(macacos[0], NULL);
    return 0;
}