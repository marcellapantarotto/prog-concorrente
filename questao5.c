// Marcella Pantarotto (13/0143880)

/*
Resolva o seguinte problema: O estacionamento de uma universidade possui 5 vagas.
Enquanto o mesmo possui vagas, não existe prioridade para quem estaciona.
Quando o mesmo está cheio, forma-se uma “fila” (imaginária), onde professores possuem a mais alta prioridade,
seguido pelos funcionários e ap´os pelos alunos. Modele a entrada e sa´ıda do estacionamento para os “professores”,
“funcionários” e “alunos” (utilize o código base disponibilizado no Aprender).
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define NP 3 //número de professores
#define NA 5 // número de alunos
#define NF 4 // núlero de funcionários

#define VAGAS 5 //capacidade

void * professor(void * meuid);
void * aluno (void * meuid);
void * funcionario (void * meuid);

pthread_mutex_t mutex_vagas = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t professor_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t funcionario_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t aluno_cond = PTHREAD_COND_INITIALIZER;

int vagas_ocupadas = 0;
int professores = 0;
int funcionarios = 0;
int alunos = 0;

void main(argc, argv)
int argc;
char *argv[];
{

  int erro;
  int i, n, m;
  int *id;

  //criar os professores
  pthread_t tP[NP];
  for (i = 0; i < NP; i++) {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tP[i], NULL, professor, (void *) (id));

    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  //criar os alunos
  pthread_t tA[NA];
  for (i = 0; i < NA; i++)
  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tA[i], NULL, aluno, (void *) (id));

    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  //criar os funcionários
  pthread_t tF[NF];
  for (i = 0; i < NF; i++){
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tF[i], NULL, funcionario, (void *) (id));
    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  //aguardar/bloquear
  pthread_join(tP[0],NULL);
}	

void * professor (void* pi){
    int id = *(int *)(pi);
    while(1){
        sleep(rand()%(id+1)+2);
        pthread_mutex_lock(&mutex_vagas);
            professores++;
            while(vagas_ocupadas == VAGAS) {
                printf("Estacionamento cheio! Professor %d vai aguardar uma vaga\n", id);
                pthread_cond_wait(&professor_cond,&mutex_vagas);
            }
            professores--;
            vagas_ocupadas++;
            printf("Professor %d: estacionei! Número de vagas ocupadas = %d, professores = %d, funcionários = %d, alunos = %d\n", id, vagas_ocupadas, professores, funcionarios, alunos);
        pthread_mutex_unlock(&mutex_vagas);
        sleep(10);  

        pthread_mutex_lock(&mutex_vagas);
            vagas_ocupadas--;
            printf("Professor %d saiu da vaga!\n", id);
            pthread_cond_signal(&professor_cond);       // signal para respeitar o tempo de estacionamento dos demais
            pthread_cond_signal(&funcionario_cond);
            pthread_cond_signal(&aluno_cond);
        pthread_mutex_unlock(&mutex_vagas);
    }
}

void * funcionario (void* pi){
    int id = *(int *)(pi);
    while(1){
        sleep(rand()%(id+1)+2);
        pthread_mutex_lock(&mutex_vagas);
            funcionarios++;
            while(vagas_ocupadas == VAGAS || professores > 0) {
                printf("Estacionamento cheio! Funcionário %d vai aguardar uma vaga\n", id);
                pthread_cond_wait(&funcionario_cond,&mutex_vagas);
            }
            funcionarios--;
            vagas_ocupadas++;
            printf("Funcionário %d: estacionei! Número de vagas ocupadas = %d, professores = %d, funcionários = %d, alunos = %d\n", id, vagas_ocupadas, professores, funcionarios, alunos);
        pthread_mutex_unlock(&mutex_vagas);
        sleep(10);  

        pthread_mutex_lock(&mutex_vagas);
            vagas_ocupadas--;
            printf("Funcionário %d saiu da vaga!, número de vagas ocupadas = %d\n", id, vagas_ocupadas);
            pthread_cond_signal(&professor_cond);       // signal para respeitar o tempo de estacionamento dos demais
            pthread_cond_signal(&funcionario_cond);
            pthread_cond_signal(&aluno_cond);
        pthread_mutex_unlock(&mutex_vagas);
    }
}

void * aluno (void* pi){
    int id = *(int *)(pi);
    while(1){
        sleep(rand()%(id+1)+2);
        pthread_mutex_lock(&mutex_vagas);
            alunos++;
            while(vagas_ocupadas == VAGAS || professores > 0 || funcionarios > 0) {
                printf("Estacionamento cheio! Aluno %d vai aguardar uma vaga\n", id);
                pthread_cond_wait(&aluno_cond,&mutex_vagas);
            }
            alunos--;
            vagas_ocupadas++;
            printf("Aluno %d: estacionei! Número de vagas ocupadas = %d, professores = %d, funcionários = %d, alunos = %d\n", id, vagas_ocupadas, professores, funcionarios, alunos);
        pthread_mutex_unlock(&mutex_vagas);
        sleep(10);  
        
        pthread_mutex_lock(&mutex_vagas);
            vagas_ocupadas--;
            printf("Aluno %d saiu da vaga!, número de vagas ocupadas = %d\n", id, vagas_ocupadas);
            pthread_cond_signal(&professor_cond);       // signal para respeitar o tempo de estacionamento dos demais
            pthread_cond_signal(&funcionario_cond);
            pthread_cond_signal(&aluno_cond);
        pthread_mutex_unlock(&mutex_vagas);
    }
}