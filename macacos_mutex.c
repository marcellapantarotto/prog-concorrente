#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MA 10 //macacos que andam de A para B
#define MB 10 //macacos que andam de B para A

// (se usar apenas um lock garante a exclusão mútua da região crítica por apenas 1 leitor ou 1 escritor)
pthread_mutex_t lock_corda = PTHREAD_MUTEX_INITIALIZER;     // lock da corda
pthread_mutex_t lock_vez = PTHREAD_MUTEX_INITIALIZER;       // lock da vez
pthread_mutex_t lock_mAB = PTHREAD_MUTEX_INITIALIZER;       // lock dos macacos de A->B
pthread_mutex_t lock_mBA = PTHREAD_MUTEX_INITIALIZER;       // lock dos macacos de B->A

int vez = 0;
int num_mAB = 0;  // número de macacos de A->B
int num_mBA = 0;  // número de macacos de B->A
int num_gAB = 1;  // número de macacos de A->B
int num_gBA = 1;  // número de macacos de B->A

void * macacoAB(void * a) {
  int i = *((int *) a);    
  while(1){
    // Procedimentos para entrar na corda
    pthread_mutex_lock(&lock_vez);
      pthread_mutex_lock(&lock_mAB); // macaco de A->B pega o lock da travessia
        num_mAB++;
        if(num_mAB == 1){
          pthread_mutex_lock(&lock_corda);
        }
      pthread_mutex_unlock(&lock_vez);
    pthread_mutex_unlock(&lock_mAB);
    
    printf("Macaco %d passando de A para B \n", i);
    sleep(1);

    // Procedimentos para quando sair da corda
    pthread_mutex_lock(&lock_mAB); // macaco de A->B pega o lock da travessia
        num_mAB--;
        if(num_mAB == 0){
          pthread_mutex_unlock(&lock_corda);
        }
    pthread_mutex_unlock(&lock_mAB); 
  }
  pthread_exit(0);
}

// leitores
void * macacoBA(void * a) {
  int i = *((int *) a);    
  while(1){
    // Procedimentos para entrar na corda
    pthread_mutex_lock(&lock_vez);
      pthread_mutex_lock(&lock_mBA); // macaco de A->B pega o lock da travessia
        num_mBA++;
        if(num_mBA == 1){
          pthread_mutex_lock(&lock_corda);
        }
      pthread_mutex_unlock(&lock_vez);
    pthread_mutex_unlock(&lock_mBA);

    printf("Macaco %d passando de B para A \n", i);
    sleep(1);

    // Procedimentos para quando sair da corda
    pthread_mutex_lock(&lock_mBA); // macaco de A->B pega o lock da travessia
        num_mBA--;
        if(num_mBA == 0){
          pthread_mutex_unlock(&lock_corda);
        }
    pthread_mutex_unlock(&lock_mBA); 
  }
  pthread_exit(0);
}

// são como os escritores então vai precisar de 2 locks nos 2 macacos e gorila
void * gorilaAB(void * a){ 
    while(1){
      //Procedimentos para acessar a corda
      pthread_mutex_lock(&lock_vez);
        pthread_mutex_lock(&lock_corda);
          printf("Gorila passado de A para B \n");
        pthread_mutex_unlock(&lock_vez);
        sleep(1);
      //Procedimentos para quando sair da corda
      pthread_mutex_unlock(&lock_corda);
      
     }
    pthread_exit(0);
}

void * gorilaBA(void * a){  
  while(1){
    //Procedimentos para acessar a corda
    pthread_mutex_lock(&lock_vez);
      pthread_mutex_lock(&lock_corda);
        printf("Gorila passado de B para A \n");
      pthread_mutex_unlock(&lock_vez);
      sleep(1);
    //Procedimentos para quando sair da corda
    pthread_mutex_unlock(&lock_corda);
    
    }
  pthread_exit(0);
}

int main(int argc, char * argv[])
{
    srand(time(NULL));
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

    pthread_t g1;
    pthread_create(&g1, NULL, &gorilaAB, NULL);

    pthread_t g2;
    pthread_create(&g2, NULL, &gorilaBA, NULL);

  
    pthread_join(macacos[0], NULL);
    return 0;
}