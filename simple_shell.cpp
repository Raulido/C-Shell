//Raul Pulido 4/18/19
//4399655 CS 170
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#define MAX 513
#define MAXX 33
void execute(char*** &args, int background, char *out, int n, char *in){
    int tmpin = dup(0);
    int tmpout = dup(1);
    pid_t pid1;
    int status;
    int fdin;
    if(in != NULL){
        fdin = open(in,O_RDONLY);
    }
    else{
        fdin = dup(tmpin);
    }
    int fdout;
    for(int a = 0; a < n; a++){
        dup2(fdin, 0);
        close(fdin);
        if(a == n-1){
            if(out != NULL){
                fdout = creat(out, 0644);
            }
            else{
                fdout = dup(tmpout);
            }
        }
        else{
            int fdpipe[2];
            pipe(fdpipe);
            fdout = fdpipe[1];
            fdin = fdpipe[0];
        }
        dup2(fdout,1);
        close(fdout);
        pid1 = fork();
        if (pid1 == 0) {
            if (execvp(args[a][0], args[a]) == -1) {
                fprintf(stderr, "ERROR: Command\n");
            }
        }
    }
    if(!background){
        waitpid(pid1,&status,0);
    }
    dup2(tmpin, 0);
    dup2(tmpout, 1);
    close(tmpin);
    close(tmpout);
}
int main(int argc, char *argv[]){
    bool status = true;
    bool noshell = true;
    do{
        if(noshell){
            printf("shell: ");
        }
        char *line = (char *)malloc(MAX * sizeof(char));
        char *fout = (char *)malloc(MAX * sizeof(char));
        char *fin = (char *)malloc(MAX * sizeof(char));
        fout = NULL;
        fin = NULL;
        char **args = (char **)malloc(MAX * sizeof(char *));
        for(int i = 0; i < MAX; i++){
            args[i] = (char *)malloc(MAXX * sizeof(char));
            args[i] = NULL;
        }
        char *** args2 = (char *** )malloc(600 * sizeof(char ** ));
        for(int i = 0; i < 600; i++){ 
            args2[i] = (char ** ) malloc(600 * sizeof(char * )) ;
            for (int j = 0; j < 600; j++){ 
                args2[i][j] = (char *) malloc (600 * sizeof(char));
                args2[i][j] = NULL;
            }
        } 
        char *** COMMANDS = (char *** )malloc(300 * sizeof(char ** ));
        for(int i = 0; i < 300; i++){ 
            COMMANDS[i] = (char ** ) malloc(300 * sizeof(char * )) ;
            for (int j = 0; j < 300; j++){ 
                COMMANDS[i][j] = (char *) malloc (32 * sizeof(char));
                COMMANDS[i][j] = NULL;
            }
        } 
        char *token;
        int num = 0;
        if(!fgets(line, MAX, stdin)){
            printf("\n");
            return 0;
        }
        strtok(line, "\n");
        if(line[0] == '|' || line[strlen(line) - 1] == '|'){
            fprintf(stderr, "ERROR: Invalid Syntax\n"); 
            continue;
        }
        int count1 = 0;
        const char *tmp = line;
        while(tmp = strstr(tmp, "<")){
            count1++;
            tmp++;
        }
        if(count1 > 1){
            fprintf(stderr, "ERROR: Invalid Syntax\n"); 
            continue;
        }
        int count2 = 0;
        const char *tmp2 = line;
        while(tmp2 = strstr(tmp2, ">")){
            count2++;
            tmp2++;
        }
        if(count2 > 1){
            fprintf(stderr, "ERROR: Invalid Syntax\n"); 
            continue;
        }
        int count3 = 0;
        const char *tmp3 = line;
        while(tmp3 = strstr(tmp3, "&")){
            count3++;
            tmp3++;
        }
        if(count3 > 1){
            fprintf(stderr, "ERROR: Invalid Syntax\n"); 
            continue;
        }
        const char *tmp4 = line;
        if(strstr(tmp4, "||") != NULL) {
            fprintf(stderr, "ERROR: Invalid Syntax\n"); 
            continue;
        }
        token = strtok(line, "|");
        while(token != NULL){
            args[num] = token;
            num++;
            token = strtok(NULL, "|");
        } 
        int size1 = 0;
        int size2[512];
        int size3[512][512];
        int second = 0;
        for(int i = 0; i < MAX; i++){
            if(args[i] != NULL){
                token = strtok(args[i], " ");
                while(token != NULL){
                    args2[i][second] = token;
                    size3[i][second] = strlen(token);
                    token = strtok(NULL, " ");
                    second++;
                }
                if(second == 0){
                    args2[i][0] = args[i];
                }
                size2[i] = second;
                second = 0;
                size1++;   
            }
        }
        int a1= 0;
        int a2 = 0;
        for(int i = 0; i < size1; i++){
            for(int j = 0; j < size2[i]; j++){
                if(args2[i][j] != NULL){
                    a1 = i;
                    a2 = j;
                }
            }
        }
        int a3 = a2 - 1;
        int lastlen = 0;
        char lastchar = '\0';
        for(int i = 0; i < size1; i++){
            for(int j = 0; j < size2[i]; j++){
                if(args2[i][j] != NULL){
                    lastlen = strlen(args2[i][j]);
                    lastchar = args2[i][j][lastlen-1];
                    args2[i][j] = strtok(args2[i][j] , "&");
                }
            }
        }
        if(count3 == 1){
            if( lastchar != '&'){
                fprintf(stderr, "ERROR: Invalid Syntax\n"); 
                continue;
            }
        }
        if (size1 == 1 && size2[0] == 1){
            if(args2[0][0][0] == '-' && args2[0][0][1] == 'n' && args2[0][0][2] == '\0'){
                noshell = 0;
                continue;
            }
        }
        if(count2 == 1){
            lastlen = 0;
            lastchar = '\0';
            for(int i = 0; i < size1; i++){
                for(int j = 0; j < size2[i]; j++){
                    if(args2[i][j] != NULL){
                        lastlen = strlen(args2[i][j]);
                        lastchar = args2[i][j][lastlen-1];
                    }
                }
            }
            if( lastchar == '>'){
                fprintf(stderr, "ERROR: No output file\n"); 
                continue;
            }
            else{
                for(int i = 0; i < size1; i++){
                    for(int j = 0; j < size2[i]; j++){
                        if(args2[i][j] != NULL){
                            if(strstr(args2[i][j], ">") != NULL) {
                                if(!((a1 == i && a2 == j) || (a1 == i && a3 == j))){
                                    fprintf(stderr, "ERROR: Bad Syntax\n"); 
                                    goto xyz;
                                }
                            }
                        }
                    }
                } 
                char *temp = (char *)malloc(MAX * sizeof(char));
                strcpy(temp, "");
                for(int i = 0; i < size1; i++){
                    for(int j = 0; j < size2[i]; j++){
                        if(args2[i][j] != NULL && i == size1 - 1){
                            strcat(temp,args2[i][j]);
                        }
                    }
                }
                char **gett = (char **)malloc(2 * sizeof(char *));
                for(int i = 0; i < 2; i++){
                    gett[i] = (char *)malloc(MAXX * sizeof(char));
                    gett[i] = NULL;
                }
                char *x2;
                int y = 0;
                x2 = strtok(temp, ">");
                while(x2 != NULL){
                    gett[y] = x2;
                    y++;
                    x2 = strtok(NULL, ">");
                }
                fout = gett[1];
                fout = strtok(fout , " ");
                const char *tmp10 = args2[a1][a2];
                char *temp11 = (char *)malloc(100 * sizeof(char));
                strcpy(temp11, "");
                strcat(temp11, ">");
                strcat(temp11, fout);
                if(strstr(tmp10, temp11) != NULL) {
                    if(strstr(temp11, tmp10) != NULL) {
                        args2[a1][a2] = NULL;
                        size2[a1] = size2[a1] - 1;
                        a2 = a2 - 1;
                        a3 = a3 - 1;
                    }
                    else{
                        strcpy(args2[a1][a2],gett[0]);
                    }
                }
                else{
                    lastlen = strlen(args2[a1][a3]);
                    lastchar = args2[a1][a3][lastlen-1];
                    if(lastchar == '>' && lastlen == 1){
                        args2[a1][a2] = NULL;
                        args2[a1][a3] = NULL;
                        a2 = a2 - 2;
                        a3 = a3 - 2;
                        size2[a1] = size2[a1] - 2;
                    }
                    else{
                        args2[a1][a2] = NULL;
                        args2[a1][a3] = strtok(args2[a1][a3] , ">");
                        a2 = a2 - 1;
                        a3 = a3 - 1;
                        size2[size1] = size2[size1] - 1;
                    }
                }
            }
        }   
        if(count1 == 1){
            lastlen = 0;
            lastchar = '\0';
            for(int j = 0; j < size2[0]; j++){
                if(args2[0][j] != NULL){
                    lastlen = strlen(args2[0][j]);
                    lastchar = args2[0][j][lastlen-1];
                }
            }
            
            if( lastchar == '<'){
                fprintf(stderr, "ERROR: No inputfile\n"); 
                continue;
            }
            else{
                int b2 = size2[0] - 1;
                int b3 = b2 - 1;
                for(int i = 0; i < size1; i++){
                    for(int j = 0; j < size2[i]; j++){
                        if(args2[i][j] != NULL){
                            if(strstr(args2[i][j], "<") != NULL) {
                                if(!((i == 0 && b2 == j) || ( i == 0 && b3 == j))){
                                    fprintf(stderr, "ERROR: Bad Syntax\n"); 
                                    goto xyz;
                                }
                            }
                        }
                    }
                }
                char *temp = (char *)malloc(MAX * sizeof(char));
                strcpy(temp, "");
                for(int i = 0; i < size2[0]; i++){
                    if(args2[0][i] != NULL){
                        strcat(temp,args2[0][i]);
                    }
                }
                char **gett = (char **)malloc(2 * sizeof(char *));
                for(int i = 0; i < 2; i++){
                    gett[i] = (char *)malloc(MAXX * sizeof(char));
                    gett[i] = NULL;
                }
                char *x2;
                int y = 0;
                x2 = strtok(temp, "<");
                while(x2 != NULL){
                    gett[y] = x2;
                    y++;
                    x2 = strtok(NULL, "<");
                }
                fin = gett[1];
                fin = strtok(fin , " ");
                const char *tmp10 = args2[0][b2];
                char *temp11 = (char *)malloc(100 * sizeof(char));
                strcpy(temp11, "");
                strcat(temp11, "<");
                strcat(temp11, fin);
                if(strstr(tmp10, temp11) != NULL) {
                    if(strstr(temp11, tmp10) != NULL) {
                        args2[0][b2] = NULL;
                        size2[0] = size2[0] - 1;
                        b2 = b2 - 1;
                        b3 = b3 - 1;
                    }
                    else{
                        strcpy(args2[0][b2],gett[0]);
                    }
                }else{
                    lastlen = strlen(args2[0][b3]);
                    lastchar = args2[0][b3][lastlen-1];
                    if(lastchar == '<' && lastlen == 1){
                        args2[0][b2] = NULL;
                        args2[0][b3] = NULL;
                        b2 = b2 - 2;
                        b3 = b3 - 2;
                        size2[0] = size2[0] - 2;
                    }
                    else{
                        args2[0][b2] = NULL;
                        args2[0][b3] = strtok(args2[0][b3] , "<");
                        b2 = b2 - 1;
                        b3 = b3 - 1;
                        size2[0] = size2[0] - 1;
                    }
                }
            }
        }
        for(int i = 0; i < size1; i++){
            for(int j = 0; j < size2[i]; j++){
                if(args2[i][j] != NULL){
                    COMMANDS[i][j] = args2[i][j];
                }
            }
        }
        execute(COMMANDS, count3, fout, size1, fin);
        for(int i = 0 ; i < size1 ; i++ ){  
            for ( int j = 0 ; j < size2[size1] ; j++ ){ 
                delete COMMANDS[i][j];
            } 
        }
        for(int i = 0 ; i < size1 ; i++ ){  
            delete COMMANDS[i];
        }
        delete COMMANDS;
        xyz:;
    }while(status);
    return 0;
}
