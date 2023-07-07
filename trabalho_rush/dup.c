#include "dup.h"
#include "types.h"
#define QUANTPIPES 3


char *verify_til(char *command){

    char *home_dir = getenv("HOME");
    char *end_til = strchr(command, '~');
    if(end_til){

        char *new_dir = aloca_memoria(NULL, sizeof(char)* strlen(home_dir) + sizeof(char)*strlen(end_til+1)+sizeof(char));

        strcpy(new_dir, home_dir);
        strcat(new_dir, end_til+1);
        free(command);
        return new_dir;

    }
    return command;
}

matCommands verify_file_in_out(char *command, char *tok, int STDINOUT){
    int fd = -1;

    char *text = strstr(command, tok);

    matCommands cm;

    if(text != NULL){

        cm = break_commands(tok, command);

        matCommands file = break_commands(" ",cm.commands[cm.lines-2]);

        file.commands[0] = verify_til(file.commands[0]);

        if(STDINOUT == STDIN_FILENO) {
            fd = open(file.commands[0], O_RDONLY);


        }else {
            if (!strcmp(tok, ">>")) {

                fd = open(file.commands[0], O_WRONLY | O_APPEND);
            } else {

                fd = open(file.commands[0], O_WRONLY | O_CREAT | O_TRUNC  , 0666);

            }

        }
        if(fd == -1){
            fprintf(stderr, STDINOUT == STDIN_FILENO?"Arquivo %s nao encontrado!\n":"Nao foi possivel abrir o arquivo %s!\n", cm.commands[cm.lines-2]);
            destructor_mat(&file);
            exit(1);
        }
        dup2(fd, STDINOUT);
        close(fd);

        destructor_mat(&file);
        return cm;
    }

    cm.lines=2;
    cm.commands = aloca_memoria(NULL, sizeof(char*)*2);
    cm.commands[0] = add_str(command);
    cm.commands[1] = NULL;
    return cm;
}

int twoCommandsBaseCase(char *command1, char *command2, char *name_exec){
    int pipes[2];
    if(pipe(pipes) == -1){
        fprintf(stderr, "Erro na criação do pipe!");
        return 1;
    }
    pid_t filho1 = fork();
    if(filho1 == -1){
        fprintf(stderr, "Erro na criação do filho!");
        return 1;
    }
    if(filho1 == 0){
        dup2(pipes[WRITE_END], STDOUT_FILENO);
        close(pipes[READ_END]);
        close(pipes[WRITE_END]);



        matCommands cmFilho1 = verify_file_in_out(command1, "<", STDIN_FILENO);

        cmFilho1 = break_commands(" ", cmFilho1.commands[0]);

        read_file(name_exec, &cmFilho1);
        cmFilho1.commands[0] = swap_string(cmFilho1.commands);
        execvp(cmFilho1.commands[0], cmFilho1.commands);
        fprintf(stderr, "Erro ao executar o comando %s", cmFilho1.commands[0]);
        exit(1);
    }

    pid_t filho2 = fork();

    if(filho2 == -1){
        fprintf(stderr, "Erro na criação do filho!");
        return 1;
    }

    if(filho2 == 0){

        dup2(pipes[READ_END], STDIN_FILENO);
        close(pipes[READ_END]);
        close(pipes[WRITE_END]);


        matCommands  cmFilho2 = verify_file_in_out(command2, ">>", STDOUT_FILENO);

        if(cmFilho2.lines == 2){
            cmFilho2 = verify_file_in_out(command2, ">", STDOUT_FILENO);
        }

        cmFilho2 = break_commands(" ", cmFilho2.commands[0]);
        read_file(name_exec, &cmFilho2);
        cmFilho2.commands[0] = swap_string(cmFilho2.commands);
        execvp(cmFilho2.commands[0], cmFilho2.commands);
        fprintf(stderr, "Erro ao executar o comando %s", cmFilho2.commands[0]);
        exit(1);
    }
    close(pipes[READ_END]);
    close(pipes[WRITE_END]);
    int status[2];
    waitpid(filho1, status,0);
    waitpid(filho2, status+1, 0);
    return status[0] || status[1];
}

int run_pipe(matCommands *commands, char *name_exec){

    int pipes[QUANTPIPES][2];
    int status[3] = {0,0,0};
    for (int i = 0; i<QUANTPIPES; i++){
        if(pipe(pipes[i]) == -1){
            fprintf(stderr, "ERRO: erro ao criar o pipe!\n");
            return 1;
        };
    }



    pid_t process[3];
    process[0] = fork();

    if(process[0] == 0){


        dup2(pipes[0][WRITE_END], STDOUT_FILENO);
        close(pipes[0][READ_END]);
        close(pipes[0][WRITE_END]);
        close(pipes[1][WRITE_END]);
        close(pipes[1][READ_END]);
        matCommands comandotemp =verify_file_in_out(commands->commands[0], "<", STDIN_FILENO);

        matCommands comando = break_commands(" ",comandotemp.commands[0]);

        read_file(name_exec, &comando);
        comando.commands[0] = swap_string(comando.commands);
        execvp(comando.commands[0], comando.commands);
        fprintf(stderr, "ERRO: Erro ao executar o comando %s!\n", comando.commands[0]);
        exit(1);




    }

    int i;
    for( i = 1; i<commands->lines-2; i++) {
        process[i%3] = fork();

        if (process[i%3] == 0) {



            close(pipes[(i-1)%QUANTPIPES][WRITE_END]);
            close(pipes[i%QUANTPIPES][READ_END]);

            dup2(pipes[(i-1)%QUANTPIPES][READ_END], STDIN_FILENO);
            dup2(pipes[i%QUANTPIPES][WRITE_END], STDOUT_FILENO);

            close(pipes[(i-1)%QUANTPIPES][WRITE_END]);
            close(pipes[i%QUANTPIPES][READ_END]);
            close(pipes[i%QUANTPIPES][WRITE_END]);
            close(pipes[(i-1)%QUANTPIPES][READ_END]);


            matCommands cm = break_commands(" ", commands->commands[i]);
            read_file(name_exec, &cm);
            cm.commands[0] = swap_string(cm.commands);
            execvp(cm.commands[0], cm.commands);
            fprintf(stderr, "ERRO: Erro ao executar o comando %s!\n", cm.commands[0]);
            exit(1);
        }

        close(pipes[(i-1)%QUANTPIPES][WRITE_END]);
        close(pipes[(i-1)%QUANTPIPES][READ_END]);
        pipe(pipes[(i-1)%QUANTPIPES]);//substitui por um novo pipe

    }
    i = (i-1)%QUANTPIPES;

    pid_t process3 = fork();

    if(process3 == 0){




        dup2(pipes[i][READ_END], STDIN_FILENO);
        for(int j = 0; j<QUANTPIPES; j++){
            close(pipes[j][READ_END]);
            close(pipes[j][WRITE_END]);
        }

        matCommands cm = verify_file_in_out(commands->commands[commands->lines-2], ">>", STDOUT_FILENO);

        if(cm.lines ==2){
            cm = verify_file_in_out(commands->commands[commands->lines-2], ">", STDOUT_FILENO);

        }
        matCommands cm1 = break_commands(" ", cm.commands[0]);

        read_file(name_exec, &cm1);
        cm1.commands[0] = swap_string(cm1.commands);
        execvp(cm1.commands[0], cm1.commands);
        fprintf(stderr, "ERRO: Erro ao executar o comando %s!\n", cm1.commands[0]);
        exit(1);



    }
    for(int i = 0; i<QUANTPIPES; i++){
        close(pipes[i][READ_END]);
        close(pipes[i][WRITE_END]);
    }


    waitpid(process[0], status, 0);
    waitpid(process[1], status+1, 0);
    waitpid(process3, status+2, 0);
    return status[0] || status[1] || status[2];

}

char *pegaUsuario(){

    char *nome = getenv("USER");

    if(nome != NULL && strcmp(nome, "")){
        nome = add_str(nome);
        return nome;
    }
    //caso em que nao foi possivel consultar o nome do usuario simplesmente consultando uma var de ambiente

    int pipes[2];

    pipe(pipes);

    nome = aloca_memoria(NULL, sizeof(char)*20);
    pid_t filho = fork();



    if(filho == 0){

        dup2(pipes[WRITE_END], STDOUT_FILENO);
        close(pipes[WRITE_END]);
        close(pipes[READ_END]);

        char *executar[2] = {"who", NULL};
        execvp(executar[0], executar);
        fprintf(stderr, "Nao foi possivel obter o nome do usuario atual!\n");
        exit(1);
    }

    close(pipes[WRITE_END]);
    int status;
    waitpid(filho, &status, 0);

    if(status == 0)
        read(pipes[READ_END], nome, 19);
    else{
        strcpy(nome, "user");
    }
    if(!strcmp(nome, "")){
        strcpy(nome, "user");
    }
    *strchr(nome, ' ') = '\0';
    return nome;
}

char *PegaCaminho (char *command){
    pid_t filho;
    int pipes[2];
    if (pipe(pipes) == -1){
        fprintf(stderr, "Erro ao criar o pipe!");
        return NULL;
    }

    filho = fork();

    if(filho == 0){

        dup2(pipes[WRITE_END], STDOUT_FILENO);
        close(pipes[WRITE_END]);
        close(pipes[READ_END]);

        char *which[3] = {"/usr/bin/which", command, NULL};
        execvp("/usr/bin/which", which);
        fprintf(stderr,"comando %s nao encontrado!\n", command);
        exit(1);
    }
    int status;
    close(pipes[WRITE_END]);
    waitpid(filho, &status, 0);

    if(!status){
        char *name = aloca_memoria(NULL,sizeof(char)*50);
        read(pipes[READ_END], name, 50);
        read(pipes[READ_END], name, 50);
        return name;
    }
    return NULL;
}
char *swap_string(char **string_or){

    char *new = PegaCaminho(*string_or);
    if (!new){
        fprintf(stderr, "Comando %s nao encontrado!\n", *string_or);
        return NULL;
    }
    if(strchr(new, '\n')){
        *strchr(new, '\n') = '\0';
    }
    *string_or = clear_memory(*string_or);
    return new;

}