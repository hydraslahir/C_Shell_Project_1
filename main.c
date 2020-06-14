#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <fcntl.h>

#define RED "\033[1;31m"
#define PURPLE "\033[1;35m"
#define NC "\e[39m"

#define LS "ls"
#define CD "cd"
#define CLEAR "clear"
#define MAN "man"

void clearArgs(char **array){

    for(int i=0; i< sizeof array; i++){
        array[i] = NULL;
    }
}

void analyseCurrent(char* arguments[], char* rest, bool debug){
    char* cwd;
    char _buffer[1024];
    cwd = getcwd(_buffer, 1024 * 8);
    /********************  CD ***************************/
    if (arguments[0] != NULL) {
        if (strcmp(arguments[0], CD) == 0) {
            if (arguments[1] != NULL) {
                int ret = chdir(arguments[1]);
                if(ret == -1){
                    fprintf(stderr,"Le dossier \"%s\" n'existe pas, veuillez réessayer\n", arguments[1]);
                }
                return;
            }else{
                system("pwd");
            }
        }
    }else{
        return;
    }

    /************************** PAUSE ************************/
    if( strcmp(arguments[0], "pause") == 0){
        char pause[10];
        while(fgets(pause,sizeof pause, stdin)){
            if( strcmp(pause, "\n")){
                break;
            }else{}
        }
    }

    /************************* QUIT *************************/
    if( strcmp(arguments[0], "quit") ==0){
        exit(-1);
    }

}


void analyseLine(char* arguments[],char* rest,bool debug){
    char* cwd;
    char _buffer[1024];
    cwd = getcwd(_buffer, 1024 * 8);



    if(arguments[0] != NULL){
/************************** CLEAR ***************************/
        if(strcmp(arguments[0], CLEAR) == 0){
            system("clear");
            exit(-1);
/***************************   LS   ****************************/
        }else if(strcmp(arguments[0], LS)==0){
                    execvp("ls",arguments);
        }
            /************************ ENVIRON **************************/
        else if( strcmp(arguments[0],"environ") ==0){
            system("env");
        }

            /************************* ECHO ***************************/
        else if( strcmp(arguments[0], "echo") == 0 ){
            if(arguments[1] != NULL){
                printf("%s\n", rest);
            }
            _exit(0);
        }
            /*************************** HELP ***********************/
        else if( strcmp(arguments[0], MAN) == 0){
            if(arguments[1] == NULL) {
                execlp("man", "man", NULL);
            }else{
                execlp("man", "man", arguments[1], NULL);
            }
        }
        exit(-1);
    }
}

int main(int argc, char* argv[]) {
    bool debug = 0;
    if(argc >=2 && (strcmp(argv[1],"---debug"))){
        debug = 1;
    }

    char ligne[100];
    char _buffer[1024];
    char* cwd;
    char* arguments[10];

    cwd = getcwd(_buffer, 1024 * 8);

    bool print = true;

    FILE *fd = stdin;
    if(argc >=2) {
        if ((fd = fopen(argv[1], "r")) == NULL) {
            fd = stdin;
        }else{
            print = false;
        }
    }

    system("clear");



    if(isatty(fileno(stdin)) && print){
        printf("%s[%s%s~%s]%s$ ", RED, "Hydraslahir@LINUX:", PURPLE, cwd, NC);
    }
    /************************************* ARGUMENTS***********************************/

    while(fgets(ligne,sizeof ligne,fd)!= NULL) {
        if (ligne[0] == '\n') {
            if (debug) printf("Empty liine\n");
        }
        // Remove \n

        char ligneCPY[100];
        strtok(ligne, "\n");
        strcpy(ligneCPY,ligne);

        strtok(ligneCPY, " ");
        char* rest = strtok(NULL,"\n");
        cwd = getcwd(_buffer, 1024 * 8);

        clearArgs(arguments);
        int i=1;
        arguments[0] = strtok(ligne," ");
        while ((arguments[i++] = strtok(NULL, " "))) {}



        analyseCurrent(arguments,rest,debug);
        if((fork() )== 0) {
            analyseLine(arguments,rest,debug);
            _exit(-1);
        }else{
            wait(NULL);
        }

        cwd = getcwd(_buffer, 1024 * 8);

        if(isatty(fileno(stdin)) && print){
            printf("%s[%s%s~%s]%s$ ", RED, "Hydraslahir@LINUX:", PURPLE, cwd, NC);
        }
    }

    return 0;
}
