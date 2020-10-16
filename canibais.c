// Marcella Pantarotto (13/0143880)

/*
Uma tribo de N canibais selvagens está jantando os missionários capturados. Eles ficam ao redor de um grande caldeirão contendo X porções de ensopado de missionário cozido.
Quando um selvagem tem fome, ele se serve no caldeirão (pega 1 porção), a menos que esteja vazio (neste caso, o canibal deve aguardar/dormir até que o cozinheiro forneça mais porções).
Quando um canibal pegar a última porção, o mesmo deve acordar o cozinheiro para que ele prepare mais X porções de ensopado e encha o caldeirão.
Após encher a tigela, o cozinheiro acorda os canibais selvagens que estavam esperando o ensopado ficar pronto e volta a dormir.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAXCANIBAIS 20

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_canibal = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t canibal_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cozinheiro_cond = PTHREAD_COND_INITIALIZER;

void *canibal(void*meuid);
void *cozinheiro(int m);

int comida = 0;   // contador

void main(argc, argv)
int argc;
char *argv[];
{
  int erro;
  int i, n, m;
  int *id;

  pthread_t tid[MAXCANIBAIS];

  if(argc != 3){
    printf("erro na chamada do programa: jantar <#canibais> <#comida>\n");
    exit(1);
  }
  
  n = atoi (argv[1]); //número de canibais
  m = atoi (argv[2]); // quantidade de porções que o cozinheiro consegue preparar por vez
printf("Numero de Canibais: %d -- Quantidade de Comida: %d\n", n, m);

  if(n > MAXCANIBAIS){
    printf("o numero de canibais e' maior que o maximo permitido: %d\n", MAXCANIBAIS);
    exit(1);
  }
  
  for (i = 0; i < n; i++)  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, canibal, (void *) (id));

    if(erro){
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  cozinheiro(m); 
}

void * canibal (void* pi){
  while(1) {
    pthread_mutex_lock(&mutex);     // canibal pega o lock
      while (comida == 0) {         // se não tiver comida
        printf("Acabou a comida! Acorda cozinheiro!\n");
        pthread_cond_signal(&cozinheiro_cond);    // acorda primeiro o cozinheiro para não acontecer que todos os canibais durmam e não ter ninguém para acordá-lo
        pthread_cond_wait(&canibal_cond, &mutex); // adormece canibal
      }
      comida--;    //pegar uma porção de comida
    pthread_mutex_unlock(&mutex);   // canibal solta o lock

    printf("Canibal %d: vou comer a porção que peguei\n", *(int *)(pi));
    sleep(2);
  }
}

void *cozinheiro (int m){
  while(1){
    int porcoes = m;
    pthread_mutex_lock(&mutex);
      
      while (comida > 0)    // enquanto tiver comida
        pthread_cond_wait(&cozinheiro_cond, &mutex);   // adormece cozinheiro
      
      // preparando comida
      printf("\nCozinheiro: vou preparar a comida\n");
      comida = porcoes;
      sleep(5);
      
      // acordar os canibais
      if (comida == porcoes){
        printf("Comida pronta! Acordem canibais!\n\n");
        pthread_cond_broadcast(&canibal_cond);
      }
    pthread_mutex_unlock(&mutex);
   }
}