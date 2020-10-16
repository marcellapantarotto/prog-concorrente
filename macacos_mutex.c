/*
Suponha que há macacos em dois morros A e B, ligados por uma ponte de corda. De tempos em tempos os macacos decidem passar para o outro morro à procura de comida.
Esta passagem é feita através da ponte de corda. Mais de um macaco pode atravessar a ponte ao mesmo tempo, mas isso só é possível se eles estiverem indo na mesma direção.
Implemente um programa que faça o controle da passagem de macacos pela ponte usando locks.
DESAFIO: Após testar o programa acima, crie agora uma nova verão do programa adicionando dois gorilas, um em cada morro.
Como os gorilas são muito pesados, eles só poderão atravessar a ponte sozinhos.
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MA 10 //macacos que andam de A para B
#define MB 10 //macacos que andam de B para A

pthread_mutex_t lock_corda = PTHREAD_MUTEX_INITIALIZER;     // lock da corda
pthread_mutex_t lock_vez = PTHREAD_MUTEX_INITIALIZER;       // lock da vez
pthread_mutex_t lock_mAB = PTHREAD_MUTEX_INITIALIZER;       // lock dos macacos de A->B
pthread_mutex_t lock_mBA = PTHREAD_MUTEX_INITIALIZER;       // lock dos macacos de B->A

int num_mAB = 0;  // número de macacos de A->B
int num_mBA = 0;  // número de macacos de B->A

void * macacoAB(void * a) {
  int i = *((int *) a);    
  while(1){
    sleep(rand() % 2);  // sleep randômico pra modificar a alternaça do output
    // Procedimentos para entrar na corda
    pthread_mutex_lock(&lock_vez);            // macacos do lado A->B pegam o lock da vez
      pthread_mutex_lock(&lock_mAB);          // pega o lock dos macacos A->B
        num_mAB++;                            // incrementa a quantidade de macacos
        if(num_mAB == 1){                     // o primeiro macaco do lado A->B pega o lock da corda
          pthread_mutex_lock(&lock_corda);    // pega o lock da corda
        }
      pthread_mutex_unlock(&lock_vez);        // macacos do lado A->B liberam o lock da vez
    pthread_mutex_unlock(&lock_mAB);          // libera o lock dos macacos A->B
    
    // travessia
    printf("Macaco %d passando de A para B \n", i);
    sleep(1);

    // Procedimentos para quando sair da corda
    pthread_mutex_lock(&lock_mAB);            // pega o lock dos macacos A->B
      num_mAB--;                            // decrementa a quantidade de macacos
      if(num_mAB == 0){                     // verifica se o último macaco está saíndo da corda
        pthread_mutex_unlock(&lock_corda);  // libera o lock da corda
      }
    pthread_mutex_unlock(&lock_mAB);          // libera o lock dos macacos A->B
  }
  pthread_exit(0);
}

void * macacoBA(void * a) {
  sleep(rand() % 2);  // sleep randômico pra modificar a alternaça do output
  int i = *((int *) a);    
  while(1){
    // Procedimentos para entrar na corda
    pthread_mutex_lock(&lock_vez);          // macacos do lado B->A pegam o lock da vez
      pthread_mutex_lock(&lock_mBA);        // pega o lock dos macacos B->A
        num_mBA++;                          // incrementa a quantidade de macacos
        if(num_mBA == 1){                   // o primeiro macaco do lado B->A pega o lock da corda
          pthread_mutex_lock(&lock_corda);  // pega o lock da corda
        }
        pthread_mutex_unlock(&lock_vez);    // macacos do lado B->A liberam o lock da vez
      pthread_mutex_unlock(&lock_mBA);      // libera o lock dos macacos B->A

    // travessia
    printf("Macaco %d passando de B para A \n", i);
    sleep(1);

    // Procedimentos para quando sair da corda
    pthread_mutex_lock(&lock_mBA);          // pega o lock dos macacos B->A
      num_mBA--;                            // decrementa a quantidade de macacos
      if(num_mBA == 0){                     // verifica se o último macaco está saíndo da corda
        pthread_mutex_unlock(&lock_corda);  // libera o lock da corda
      }
    pthread_mutex_unlock(&lock_mBA);        // libera o lock dos macacos B->A
  }
  pthread_exit(0);
}

void * gorilaAB(void * a){
  sleep(rand() % 2);  // sleep randômico pra modificar a alternaça do output
  while(1){
    //Procedimentos para acessar a corda
    pthread_mutex_lock(&lock_vez);      // gorila do lado A->B pega o lock da vez
      pthread_mutex_lock(&lock_corda);  // pega lock da corda
    pthread_mutex_unlock(&lock_vez);    // libera o lock da vez
    
    // travessia
    printf("Gorila passado de A para B \n");
    sleep(5);

    //Procedimento para quando sair da corda
    pthread_mutex_unlock(&lock_corda);   // libera o lock da corda
  }
  pthread_exit(0);
}

void * gorilaBA(void * a){ 
  sleep(rand() % 2);  // sleep randômico pra modificar a alternaça do output
  while(1){
    //Procedimentos para acessar a corda
    pthread_mutex_lock(&lock_vez);      // gorila do lado B->A pega o lock da vez
      pthread_mutex_lock(&lock_corda);  // pega lock da corda
    pthread_mutex_unlock(&lock_vez);    // libera o lock da vez

    // travessia
    printf("Gorila passado de B para A \n");
    sleep(5);

    //Procedimento para quando sair da corda
    pthread_mutex_unlock(&lock_corda);   // libera lock da corda
  }
  pthread_exit(0);
}

int main(int argc, char * argv[])
{
    // srand(time(NULL));
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