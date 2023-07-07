
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dup.h"
#include "types.h"
#include <time.h>

#include <sys/mman.h>
#include <sys/stat.h>
void change_directory(char *text){

    matCommands cm = break_commands(" ", text);

    if(cm.lines == 2){
        cm.commands[1] = add_str(getenv("HOME"));
        cm.lines++;
        cm.commands = aloca_memoria(cm.commands, sizeof(char*)*cm.lines);
        cm.commands[cm.lines-1] = NULL;
    }

    cm.commands[1] = verify_til(cm.commands[1]);

    if(chdir(cm.commands[1]) == -1){

        printf("Diretorio inexistente: %s\n", cm.commands[1]);

    }
    destructor_mat(&cm);


}

int oneCommandBaseCase (char *commands, char *name_exec) {

    pid_t filho = fork();

    if(filho == 0){
        commands = add_str(commands);
        char *commandCopia = add_str(commands);
        matCommands baseCaseMinor = verify_file_in_out(commands, "<", STDIN_FILENO);
        matCommands baseCaseMax = verify_file_in_out(commandCopia, ">>", STDOUT_FILENO);

        if(baseCaseMax.lines == 2){
            baseCaseMax = verify_file_in_out(commandCopia, ">", STDOUT_FILENO);
        }

        if(!strchr(baseCaseMax.commands[0], '>') && !strchr(baseCaseMax.commands[0], '<')){

            baseCaseMax = break_commands(" ", baseCaseMax.commands[0]);
            read_file(name_exec, &baseCaseMax);
            baseCaseMax.commands[0] = swap_string(baseCaseMax.commands);
            execvp(baseCaseMax.commands[0], baseCaseMax.commands);
        } else {
            baseCaseMinor = break_commands(" ", baseCaseMinor.commands[0]);
            read_file(name_exec, &baseCaseMinor);
            baseCaseMinor.commands[0] = swap_string(baseCaseMinor.commands);
            execvp(baseCaseMinor.commands[0], baseCaseMinor.commands);
        }
        fprintf(stderr, "Erro! Falha ao executar o comando!\n");
        exit(1);
    }
    int status;
    waitpid(filho, &status, 0);

    return status;
}

int read_file(char *name_exec,matCommands *ptr){

    if (strstr(ptr->commands[0], ".rsh")) {

        char *mat[3] = {name_exec, ptr->commands[0], NULL};
        execvp(name_exec, mat);
        exit(1);
    }
    return -1;

}

int main(int argc, char **argv) {

    if(*argv[0] == '.'){
        char dirAt[100];
        getcwd(dirAt, sizeof(dirAt));
        char *mat = aloca_memoria(NULL, sizeof(char)* strlen(dirAt)+sizeof(char) + strlen(argv[0]+1)*sizeof(char));
        strcpy(mat, dirAt);
        strcat(mat, argv[0]+1);
        argv[0] = mat;

    }

    matCommands commands;
    char *command = aloca_memoria(NULL,sizeof(char)*512);
    struct tm horario;
    time_t segundos;
    char string_formated_hour[20];
    char *nomeUsuario;

    char diretorioAt[100];

    if(argc > 1){
        int fd = open(argv[1], O_RDONLY);
        if(fd == -1){
            fprintf(stderr, "Impossivel encontrar o arquivo %s!\n", argv[1]);

        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }

    while(1) {


        nomeUsuario = pegaUsuario();
        getcwd(diretorioAt, sizeof(diretorioAt));
        time(&segundos);
        horario = *localtime(&segundos);
        strftime(string_formated_hour, sizeof(string_formated_hour), "%D-%H:%M:%S", &horario);

        printf((argc  == 1)?"%s-%s:%s$ ":"", nomeUsuario, string_formated_hour, diretorioAt);
        nomeUsuario = clear_memory(nomeUsuario);

        if(argc ==1) {
            fgets(command, 512, stdin);
            *strchr(command, '\n') = '\0';
        }else{

            struct stat file_stat;
            if(fstat(STDIN_FILENO, &file_stat) == -1){
                fprintf(stderr, "ERRO: erro ao tentar ler o arquivo %s!\n", argv[1]);
                exit(1);
            }
            command = clear_memory(command);
            command = mmap(NULL, file_stat.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, STDIN_FILENO, 0);
            if (command == MAP_FAILED){
                fprintf(stderr, "ERRO: erro ao tentar ler o arquivo %s!\n", argv[1]);
                exit(1);
            }
            char *enter = strchr(command, '\n');
            while(enter!=NULL){
                *enter = ' ';
                enter = strchr(enter, '\n');
            }
        }

        matCommands ponto_e_virgula = break_commands(";", command);


        for (int i = 0; i < ponto_e_virgula.lines-1; i++) {

            if(!strcmp(ponto_e_virgula.commands[i], " ")){
                continue;
            }else if (*ponto_e_virgula.commands[i] == '\0') {
                continue;
            } else if (strstr(ponto_e_virgula.commands[i], "cd")) {
                change_directory(ponto_e_virgula.commands[i]);
                continue;
            } else if (!strcmp(ponto_e_virgula.commands[i], "exit")) {
                exit(0);
            }


            commands = break_commands("|", ponto_e_virgula.commands[i]);


            if (commands.lines > 3) {
                run_pipe(&commands, argv[0]);
            } else if (commands.lines == 3) {
                twoCommandsBaseCase(commands.commands[0], commands.commands[1], argv[0]);
            } else {

                oneCommandBaseCase(commands.commands[0], argv[0]);
            }

            destructor_mat(&commands);

        }

        if(argc > 1){
            exit(0);
        }
    }


}