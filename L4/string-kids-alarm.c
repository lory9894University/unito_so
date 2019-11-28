#define _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <strings.h>

int bill, n_child;
pid_t * children;

void printstring(char * string, int leng){
    for (int i = 0; i < leng; ++i)
    {
        printf("%c",string[i]);
    }
    printf("\n");
}

void handler(int signal){
    srand(getpid());
    bill = (rand() % n_child);
    kill(children[bill],SIGINT);
}

void child();
void father();

int main(int argc, char  const *argv[])
{
    int i=0;
    int f_value=-1;
    struct sigaction sa;

    bzero(&sa, sizeof(sa));
    sa.sa_handler= handler;
    sa.sa_flags = SA_RESTART;

    sigaction(SIGALRM,&sa,NULL);

    n_child = atoi(argv[1]);

    children = malloc(sizeof(*children) *  n_child);

    for ( i= 0; i < n_child && f_value!=0; ++i)
    {
        f_value=fork();
        children[i] = f_value;
    }
    if (f_value==0){
        child();
    }else{
        father(children);
    }
    return 0;
}

void father(){
    char * string;
    int strleng,str_act=0;
    int status=0,exit_sum = 0;
    int f_value=-1;

    string = malloc(sizeof(char) * n_child);
    strleng = n_child;

    alarm(1);
    wait(&status);
    exit_sum += status >> 8;
    string[str_act]= status >> 8;
    str_act++;

    printstring(string,str_act);

    while(exit_sum % 256 != 0){
        f_value=fork();
        if (f_value == 0)
            child();
        else {
            children[bill] = f_value;
            alarm(1);
            wait(&status);
            if (str_act == strleng) {
                strleng *= 2;
                string = realloc(string, sizeof(char) * strleng);
            }
            exit_sum += status >> 8;
            string[str_act] = status >> 8;
            str_act++;

            printstring(string, str_act);
        }

    }
    if (f_value != 0) {
        for (int i = 0; i < n_child; ++i)
            kill(children[i], SIGINT);

        while (wait(NULL) != -1);
        printf("%d\n", exit_sum );
    }

}

void child(){
    char *args[] = {"ls","Hello",NULL};
    execve("./char-loop",args,NULL);
}