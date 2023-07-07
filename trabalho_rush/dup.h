//
// Created by vitor on 15/04/23.
//

#ifndef RUSH_DUP_H
#define RUSH_DUP_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "types.h"
#define READ_END    0
#define WRITE_END	1

int read_file(char *name_exec,matCommands *ptr);
int create_dup_stdout(char *command, int FDWrite);
int run_pipe_dublo(int *pipes, int *pipes2,char *command, char *command2, int fdDest, int ler_stdin, int write_std);
matCommands verify_file_in_out(char *command, char *tok, int STDINOUT);
int exec_one_command(char *command);
int twoCommandsBaseCase(char *command1, char *command2, char *name_exec);
int run_pipe(matCommands *commands, char *name_Exec);
char *pegaUsuario();
char *verify_til(char *command);
matCommands verify_file_in_out(char *command, char *tok, int STDINOUT);
char *PegaCaminho (char *command);
char *swap_string(char **string_or);

#endif //RUSH_DUP_H