//
// Created by vitor on 08/06/23.
//

#ifndef TRABALHO_SO_PTHREAD_MONITOR_THREADS_H
#define TRABALHO_SO_PTHREAD_MONITOR_THREADS_H
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NUM_TREADS 6
#define TAM_FILA NUM_TREADS+1
#define PROXIMO 1
#define ANTERIOR -1
/*TAD fila espera*/


typedef enum classificacao{
    INVALID,CALOURO, VETERANO, SENIOR
}classificacao;

typedef struct thread{
    classificacao classe;
    pthread_cond_t *cond;
    int vezes_que_cedeu_lugar;

}thread;


typedef struct fila{
    thread esperando[TAM_FILA];
    int inicio;
    int fim;
    int quantidade;
    pthread_mutex_t mu_fila;
}fila;


/*TAD monitor*/

typedef struct monitor{

    fila threads_wait;

    pthread_mutex_t mu_forno;
    int iter_at;
    int iteracoes;



}monitor;
int eh_cheia(fila *self);
thread get_queue(fila *self);
monitor Create_monitor(int iteracoes);
void pega(monitor *self, classificacao classe, char *nome);
void devolve(monitor *self, char *nome);
int to_queue(fila *self, thread new);
thread create_thread(classificacao classe);
#endif //TRABALHO_SO_PTHREAD_MONITOR_THREADS_H
