//
// Created by vitor on 15/04/23.
//

#ifndef RUSH_TYPES_H
#define RUSH_TYPES_H

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>



typedef struct matCommands {

    char **commands;
    int lines;


} matCommands;
void *clear_memory(void *ptr);

void *aloca_memoria(void *ptr, size_t tam);
char *add_str(const char *str);
void destructor_mat(matCommands *ptr);
matCommands break_commands(char *token, char *commands);

#endif //RUSH_TYPES_H