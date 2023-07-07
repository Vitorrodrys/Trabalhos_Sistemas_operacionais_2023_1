#include "monitor_threads.h"
/*operacoes fila espera*/
int get_indice(int qual, int indice);
#define is_full(fila) fila->inicio == get_indice(self->fim, PROXIMO)
#define is_void(fila) fila->inicio == fila->fim
fila create_queue();
int to_queue(fila *self, thread new);
thread get_queue(fila *self);


void *aloca_memoria(void *ptr, size_t tam, char *msg_err){

    void *ptr_reall = realloc(ptr, tam);

    if(!ptr_reall){
        fprintf(stderr, "%s\n", msg_err);
        exit(1);
    }
    return ptr_reall;
}
void limpa_memoria(void **ptr){
    free(*ptr);
    *ptr= NULL;

}

//TAD thread
thread create_thread(classificacao classe){
    char *class[4] = {
            "","senior", "veterano", "calouro"
    };
    char *msg = aloca_memoria(NULL, sizeof(char)*strlen("Nao foi possivel criar uma nova variavel de condicao para a thread de classe ") + sizeof(char), "Erro ao alocar memoria");
    strcpy(msg, "Nao foi possivel criar uma nova variavel de condicao para a thread de classe ");
    msg = aloca_memoria(
            msg,
            sizeof(char)*strlen("Nao foi possivel criar uma nova variavel de condicao para a thread de classe ") + sizeof(char) +
            sizeof(char)* strlen(class[classe]) + sizeof (char),
            "Erro ao alocar memoria"
            );
    strcat(msg, class[classe]);


    thread new = (thread){ .classe =classe,
                     .vezes_que_cedeu_lugar=0,
                     .cond = aloca_memoria(NULL, sizeof(pthread_cond_t), msg),

        };
    pthread_cond_init(new.cond, NULL);
    limpa_memoria(&msg);
    return new;

}

thread destruct_thread(thread *self){

    limpa_memoria(&self->cond);
    return (thread){
        .classe=INVALID,
        .vezes_que_cedeu_lugar=-1,
        .cond=NULL
    };
}


//TAD fila de threads
int get_indice(int qual, int indice){

    int ind_new =  (qual+indice)%NUM_TREADS;
    if (ind_new == -1){
        return NUM_TREADS-1;
    }
    return ind_new;
}

int eh_cheia(fila *self){


    pthread_mutex_lock(&self->mu_fila);
        int resultado = self->inicio == get_indice(self->fim, PROXIMO);
    pthread_mutex_unlock(&self->mu_fila);
    return resultado;

}
int eh_vazia(fila *self){
    pthread_mutex_lock(&self->mu_fila);
        int result =self->inicio == self->fim;
    pthread_mutex_unlock(&self->mu_fila);
    return result;
}

fila create_queue(){
    fila new = (fila){
        .esperando={0},
        .inicio=0,
        .fim = 0,
        .quantidade = 0
    };
    pthread_mutex_init(&new.mu_fila, NULL);
    return new;


}


int to_queue(fila *self, thread new){
    int indice;


    pthread_mutex_lock(&self->mu_fila);

    {

        if(is_full(self)){
            pthread_mutex_unlock(&self->mu_fila);
            return 0;
        }

        indice = self->fim;

        while(
                self->esperando[get_indice(indice, ANTERIOR)].classe  < new.classe &&
                self->inicio != indice
        ){

            self->esperando[indice] = self->esperando[get_indice(indice, ANTERIOR)];
            self->esperando[indice].vezes_que_cedeu_lugar++;
            if(self->esperando[indice].vezes_que_cedeu_lugar == 2){
                self->esperando[indice].classe++;
                self->esperando[indice].vezes_que_cedeu_lugar = 0;
            }

            indice = get_indice(indice, ANTERIOR);
        }


        self->esperando[indice] = new;
        self->fim = get_indice(self->fim, PROXIMO);
        self->quantidade++;

        pthread_cond_wait(new.cond, &self->mu_fila);

    }pthread_mutex_unlock(&self->mu_fila);
    destruct_thread(&new);
    return 1;

}

thread get_queue(fila *self){

    thread retirado;
    pthread_mutex_lock(&self->mu_fila);
    {
        if (is_void(self)) {

            pthread_mutex_unlock(&self->mu_fila);
            return (thread) {INVALID, NULL};
        }
        retirado = self->esperando[self->inicio];

        self->inicio = get_indice(self->inicio, PROXIMO);
        self->quantidade--;
    }
    pthread_cond_signal(retirado.cond);
    pthread_mutex_unlock(&self->mu_fila);

    //sleep(2);



    return retirado;
}

monitor Create_monitor(int iteracoes){
    monitor new = (monitor){
                    .threads_wait = {
                        .esperando={
                            {.classe=INVALID, .vezes_que_cedeu_lugar=0},
                            {.classe=INVALID, .vezes_que_cedeu_lugar=0},
                            {.classe=INVALID, .vezes_que_cedeu_lugar=0},
                            {.classe=INVALID, .vezes_que_cedeu_lugar=0},
                            {.classe=INVALID, .vezes_que_cedeu_lugar=0},
                            {.classe=INVALID, .vezes_que_cedeu_lugar=0},
                            {.classe=INVALID, .vezes_que_cedeu_lugar=0}
                    },
                    .fim=0,
                    .inicio=0,
                    .quantidade=0,

            },
            .iteracoes=iteracoes,
            .iter_at=0

    };
    pthread_mutex_init(&new.mu_forno, NULL);


    return new;
}

void pega(monitor *self, classificacao classe, char *nome){

    if (pthread_mutex_trylock(&self->mu_forno)) {
        printf("%s entra na fila\n", nome);
        to_queue(&self->threads_wait, create_thread(classe));
    }



}
void devolve(monitor *self, char *nome){


    if (!eh_vazia(&self->threads_wait)) {
        thread retirado = get_queue(&self->threads_wait);


        return;
    }
    printf("%s libera o forno\n", nome);
    pthread_mutex_unlock(&self->mu_forno);


}
