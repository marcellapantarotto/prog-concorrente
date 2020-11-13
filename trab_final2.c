#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define N 3             // quantidade de grupos e de elementos no grupo
#define MAXLEITE 50     // quantidade de leite produzido pela vaca
#define MAXVENDEDOR 1   // quantidade de vendedores de quijo

int leite[N] = {0};    // array para contar de leite cada vaca
int bQuer = 0;      // bezerro quer

pthread_t vacas[N];         // threads de vacas
pthread_t bezerros[N];      // threads de bezerros
pthread_t funcionarios[N];  // threads de funcionários
pthread_t vendedor[MAXVENDEDOR];         // thread do vendedor

pthread_cond_t vaca_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t bezerro_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t funcionario_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t ubere = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t fabrica_quiejos = PTHREAD_MUTEX_INITIALIZER;

sem_t queijo;

void *vaca(void *arg);
void *bezerro(void *arg);
void *funcionario(void *arg);
void *loja(void *arg);

int main(int argc, char**argv) {
    int i;
    int *id;
    // srand48(time(NULL));

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

    sem_init(&queijo, 0, 0);        // semáforo de queijos
    // criando threads de vendedores
    for (i = 0; i < MAXVENDEDOR; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&vendedor[i], NULL, loja, (void *)(id));
    }

    pthread_join(vacas[0], NULL);
    pthread_join(bezerros[0], NULL);
    pthread_join(funcionarios[0], NULL);
    pthread_join(vendedor[0], NULL);

    sem_destroy(&queijo);       // destuíndo semáforo de queijos
    return 0;
}

void *vaca(void *arg) {
    int id = *((int *) arg);    // id da vaca
    int grupo = id%N;           // identificador do grupo
    printf("\tGrupo %d - Vaca %d criada\n", grupo, id);

    while(1) {
        sleep(drand48()*3);

        pthread_mutex_lock(&ubere);
            while(leite[grupo] > 5) {      // enquanto houver leite
                pthread_cond_wait(&vaca_cond, &ubere);  // adormece vaca
            }
            leite[grupo] = MAXLEITE;
            printf("-- Vaca %d pastando\n", id);
            sleep(2);

            if (leite[grupo] == MAXLEITE) {
                printf("Grupo %d - Vaca %d terminou de pastar\t\tleite remanente no grupo: %d\n", grupo, id, leite[grupo]);
                pthread_cond_signal(&bezerro_cond);
                pthread_cond_signal(&funcionario_cond);
            }
        pthread_mutex_unlock(&ubere);
    }
}

void *bezerro(void *arg) {
    int id = *((int *) arg);    // id do bezerro
    int grupo = id%N;           // identificador do grupo
    int leite_bebido = 0;       // contador de todo o leite que foi mamado
    printf("\tGrupo %d - Bezerro %d criado \n", grupo, id);

    while(1) {
        sleep(drand48()*3);

        pthread_mutex_lock(&ubere);
            bQuer++;
            while(leite[grupo] < 10) {
                printf("\tNão há leite suficiente! Bezerro %d quer mamar!\n", id);
                pthread_cond_signal(&vaca_cond);            // acorda vaca
                pthread_cond_wait(&bezerro_cond, &ubere);   // adormece bezerro
            }
            bQuer--;
            leite[grupo] = leite[grupo] - 10;
            leite_bebido = leite_bebido + 10;
            printf("Grupo %d - Bezerro %d acabou de mamar\t\tleite remanente no grupo: %d\ttotal de leite bebido: %d\n", grupo,id, leite[grupo], leite_bebido);
        pthread_mutex_unlock(&ubere);
        sleep(3);
    }
}

void *funcionario(void *arg) {
    int id = *((int *) arg);    // id do funcionario
    int grupo = id%N;           // identificador do grupo
    int leite_retirado = 0;     // contador de todo o leite ordenhado
    int value = 0;
    printf("\tGrupo %d - Funcionario %d criado \n", grupo, id);

    while(1) {
        sleep(drand48()*3);
        pthread_mutex_lock(&ubere);
            while(leite[grupo] < 5 || bQuer > 0) {
                printf("\tNão há leite suficiente! Funcionario %d precisa ordenhar!\n", id);
                pthread_cond_signal(&vaca_cond);            // acorda vaca
                pthread_cond_wait(&funcionario_cond, &ubere);   // adormece bezerro
            }

            leite[grupo] = leite[grupo] - 5;
            leite_retirado = leite_retirado + 5;
            printf("Grupo %d - Funcionário %d acabou de ordenhar\tleite remanente no grupo: %d\ttotal de leite retirado: %d\n", grupo, id, leite[grupo], leite_retirado);
        pthread_mutex_unlock(&ubere);
        sleep(1);

        while (leite_retirado >= 20) {
            // pthread_mutex_lock(&fabrica_quiejos);
            sem_post(&queijo);
            sem_getvalue(&queijo, &value);
            leite_retirado = leite_retirado - 20;
            printf("Grupo %d - FAZENDO QUEIJO!\tleite remanente no grupo: %d\ttotal de quejo: %d\n", grupo, leite[grupo], value);
            // pthread_mutex_unlock(&fabrica_quiejos);
        }
    }
}

void *loja(void *arg) {
    int queijos_vendidos = 0;
    int value = 0;

    while(1) {
        sem_wait(&queijo);
        sem_getvalue(&queijo, &value);

        // pthread_mutex_lock(&fabrica_quiejos);
        queijos_vendidos++;
        printf("** Venda feita! Total de queijos vendidos: %d, queijo remanente: %d\n", queijos_vendidos, value);
        // pthread_mutex_unlock(&fabrica_quiejos);
    }
}