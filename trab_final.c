#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define N 3     // quantidade de grupos e de elementos no grupo
#define L 30    // quantidade de leite produzido pela vaca
#define V 1     // quantidade de vendedores de quijo
#define F 1     // quantidade de fazendeiros
#define LQ 15   // quantidade de leite necessário para fazer 1 queijo

int leite[N] = {0};    // array para contar de leite cada vaca
int bQuer = 0;         // variável para indicar a preferência do bezerro

pthread_t vacas[N];            // threads de vacas
pthread_t bezerros[N];         // threads de bezerros
pthread_t funcionarios[N];     // threads de funcionários
pthread_t vendedor[V];         // thread do vendedor

pthread_cond_t vaca_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t bezerro_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t funcionario_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t ubere = PTHREAD_MUTEX_INITIALIZER;

sem_t queijos;

void *vaca(void *arg);
void *bezerro(void *arg);
void *funcionario(void *arg);
void *venda(void *arg);

int main(int argc, char**argv) {
    int i;
    int *id;
    
    sem_init(&queijos, 0, 0);        // semáforo de queijos

    // criando threads de vacas
    for (i = 0; i < N; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&vacas[i], NULL, vaca, (void *)(id));
    }

    // criando threads de bezerros
    for (i = 0; i < N; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&bezerros[i], NULL, bezerro, (void *)(id));
    }

    // criando threads de funcionários
    for (i = 0; i < N; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&funcionarios[i], NULL, funcionario, (void *)(id));
    }

    // criando threads do vendedor
    for (i = 0; i < V; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&vendedor[i], NULL, venda, (void *)(id));
    }

    pthread_join(vacas[0], NULL);
    // pthread_join(bezerros[0], NULL);
    // pthread_join(funcionarios[0], NULL);
    // pthread_join(vendedor[0], NULL);

    sem_destroy(&queijos);       // destuíndo semáforo de queijos
    return 0;
}

void *vaca(void *arg) {
    int id = *((int *) arg);    // id da vaca
    int grupo = id%N;           // identificador do grupo
    printf("\tGrupo %d - Vaca %d criada\n", grupo, id);

    while(1) {
        pthread_mutex_lock(&ubere);
            while(leite[grupo] > 5) {      // enquanto houver leite
                pthread_cond_wait(&vaca_cond, &ubere);  // adormece vaca
            }
            printf("-- Vaca %d pastando\n", id);
            leite[grupo] = L;
            sleep(2);

            printf("Grupo %d - Vaca %d terminou de pastar\t\t\tTotal de leite no grupo: %d\n", grupo, id, leite[grupo]);
            pthread_cond_signal(&bezerro_cond);
            pthread_cond_signal(&funcionario_cond);
        pthread_mutex_unlock(&ubere);
    }
}

void *bezerro(void *arg) {
    int id = *((int *) arg);    // id do bezerro
    int grupo = id%N;           // identificador do grupo
    int leite_bebido = 0;       // contador de todo o leite que foi mamado
    printf("\tGrupo %d - Bezerro %d criado \n", grupo, id);

    while(1) {
        sleep(5+rand()%5);
        pthread_mutex_lock(&ubere);
            bQuer++;
            while(leite[grupo] < 10) {   
                pthread_cond_signal(&vaca_cond);            // acorda vaca
                pthread_cond_wait(&bezerro_cond, &ubere);   // adormece bezerro
                printf("\tNão há leite suficiente! Bezerro %d quer mamar!\n", id);
            }
            bQuer--;
            leite[grupo] = leite[grupo] - 10;
            leite_bebido = leite_bebido + 10;
            printf("Grupo %d - Bezerro %d acabou de mamar\t\t\tLeite remanescente no grupo: %d\t\tTotal de leite bebido: %d\n", grupo,id, leite[grupo], leite_bebido);
        pthread_mutex_unlock(&ubere);
    }
}

void *funcionario(void *arg) {
    int id = *((int *) arg);    // id do funcionario
    int grupo = id%N;           // identificador do grupo
    int leite_retirado = 0;     // contador de todo o leite ordenhado
    int value = 0;
    printf("\tGrupo %d - Funcionario %d criado \n", grupo, id);

    while(1) {
        sleep(rand()%3);
        pthread_mutex_lock(&ubere);
            while(leite[grupo] < 5 || bQuer > 0 ) {
                pthread_cond_signal(&vaca_cond);            // acorda vaca
                pthread_cond_wait(&funcionario_cond, &ubere);   // adormece bezerro
                printf("\tNão há leite suficiente! Funcionário %d precisa ordenhar!\n", id);
            }

            leite[grupo] = leite[grupo] - 5;
            leite_retirado = leite_retirado + 5;
            printf("Grupo %d - Funcionário %d acabou de ordenhar\t\tLeite remanescente no grupo: %d\t\tTotal de leite retirado: %d\n", grupo, id, leite[grupo], leite_retirado);
        pthread_mutex_unlock(&ubere);

        while (leite_retirado >= LQ) {
            sem_post(&queijos);
            leite_retirado = leite_retirado - LQ;
            sem_getvalue(&queijos, &value);
            printf("* Grupo %d - Fazendo QUEIJO!\t\t\t\tLeite remanescente no grupo: %d\t\tTotal de queijo: %d\n", grupo, leite[grupo], value);
        }
    }
}

void *venda(void *arg) {
    int queijos_vendidos = 0;
    int value = 0;
    printf("\tVendedor criado! \n");

    while(1) {
        sleep(10+rand()%5);
        sem_wait(&queijos);
        queijos_vendidos++;
        sem_getvalue(&queijos, &value);
        printf("** VENDA FEITA!\t\t\t\t\t\tQueijos remanescentes: %d\t\tTotal de queijos vendidos: %d\n", value, queijos_vendidos);
    }
}