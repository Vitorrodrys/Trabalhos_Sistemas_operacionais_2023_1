#include <stdio.h>
#include "monitor_threads.h"
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
typedef struct args_threads{
    char *nome;
    classificacao classe;
    monitor *monitor_t;

}args_threads;





void funcao_threads(args_threads *ptr){
    int tempo_uso;


    while(ptr->monitor_t->iter_at < ptr->monitor_t->iteracoes){



        pega(ptr->monitor_t, ptr->classe, ptr->nome);
        tempo_uso = rand()%5+1;
        printf("%s esta usando o forno\n", ptr->nome);
        sleep(tempo_uso);
        ptr->monitor_t->iter_at++;
        devolve(ptr->monitor_t, ptr->nome);


    }
};

int main(int argc, char** argv) {


    monitor monitor_threads;

    srand(0);
    monitor_threads = (argc > 1)?Create_monitor(atoi(argv[1])): Create_monitor(100);


    //void (*f)(void) = teste;
    //f();

    pthread_t  threads[NUM_TREADS];
    args_threads args[NUM_TREADS] = {
            {.nome = "Saulo", .classe=SENIOR, .monitor_t=&monitor_threads},
            {.nome="Vera", .classe=VETERANO, .monitor_t=&monitor_threads},
            {.nome="Carlos",.classe=CALOURO, .monitor_t=&monitor_threads},
            {.nome="Sandro", .classe=SENIOR, .monitor_t=&monitor_threads},
            {.nome="Vani", .classe=VETERANO, .monitor_t=&monitor_threads},
            {.nome="Camila", .classe=CALOURO, .monitor_t=&monitor_threads}
    };


    for(int i = 0; i<6; i++){
        pthread_create(threads+i, NULL,(void *)funcao_threads, args +i);
    }




    pthread_join(*threads, NULL);

    return 0;
}
