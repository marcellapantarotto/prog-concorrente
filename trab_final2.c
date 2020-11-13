#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define N 3     // quantidade de grupos e de elementos no grupo
#define MAXLEITE 50

int leite[N] = {0};    // array para contar de leite cada vaca
int bQuer = 0;      // bezerro quer

pthread_t vacas[N];         // thread de vacas
pthread_t bezerros[N];      // thread de bezerros
pthread_t funcionarios[N];  // thread de funcionários

pthread_cond_t vaca_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t bezerro_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t funcionario_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t ubere = PTHREAD_MUTEX_INITIALIZER;

void *vaca(void *arg);
void *bezerro(void *arg);
void *funcionario(void *arg);

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

    pthread_join(vacas[0], NULL);
    pthread_join(bezerros[0], NULL);
    pthread_join(funcionarios[0], NULL);

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
            sleep(5);

            if (leite[grupo] == MAXLEITE) {
                printf("Vaca %d terminou de pastar\t\tleite remanente[%d]: %d\n", id, grupo, leite[grupo]);
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
            printf("Bezerro %d acabou de mamar\t\tleite remanente[%d]: %d\tleite bebido: %d\n", id, grupo, leite[grupo], leite_bebido);
        pthread_mutex_unlock(&ubere);
        sleep(6);
    }
}

void *funcionario(void *arg) {
    int id = *((int *) arg);    // id do funcionario
    int grupo = id%N;           // identificador do grupo
    int leite_retirado = 0;     // contador de todo o leite ordenhado
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
            printf("Funcionário %d acabou de ordenhar\tleite remanente[%d]: %d\tleite retirado: %d\n", id, grupo, leite[grupo], leite_retirado);
        pthread_mutex_unlock(&ubere);
        sleep(2);
    }
}
