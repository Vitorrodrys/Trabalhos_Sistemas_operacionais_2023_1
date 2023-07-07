#include "types.h"


char *add_str(const char *str) {

    char *add = aloca_memoria(NULL, sizeof(char) * strlen(str)+sizeof(char));
    strcpy(add, str);
    return add;
}
void *clear_memory(void *ptr){
    free(ptr);
    return NULL;
}
void destructor_mat(matCommands *ptr){
    for(int i = 0; i<ptr->lines-1; i++){
        ptr->commands[i] = clear_memory(ptr->commands[i]);

    }
    ptr->commands = clear_memory(ptr->commands);

}

void *aloca_memoria(void *ptr, size_t tam) {

    void *ptrRealloc;


    ptrRealloc = realloc(ptr, tam);

    if (ptrRealloc == NULL) {
        exit(1);
    }
    return ptrRealloc;


}

matCommands break_commands(char *token, char *commands){

    matCommands co;
    co.lines = 0;
    co.commands = NULL;

    commands = strtok(commands, token);

    while(commands != NULL){

        co.commands = aloca_memoria(co.commands, sizeof(char *)*(co.lines+1));
        co.commands[co.lines] = add_str(commands);
        co.lines++;
        commands =  strtok(NULL, token);

    }

    co.lines++;
    co.commands = aloca_memoria(co.commands, co.lines*sizeof(char*));
    co.commands[co.lines-1] = NULL;


    return co;

}

