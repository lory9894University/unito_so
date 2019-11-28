#include <stdio.h>
#define _GNU_SOURCE    /* NECESSARY from now on */
#include <unistd.h>
#include <string.h>
#include <signal.h> /* per usare sigaction(), sig*() */
#include <stdlib.h>
#include <sys/wait.h>

void stampa(char *str,int lung);

int main(int argc, char *argv[]){

    int n_kids, a=0,cont=0;
    int uscita, i;
    pid_t *pid;
    pid_t fork_value=-1;
    char *s;
    char *args[]={"./char-loop","gfg",NULL};

    n_kids=atoi(argv[1]);
    pid=malloc(sizeof(pid_t)*n_kids);
    s=malloc(sizeof(char)*n_kids);

    while(n_kids>0){
        if(fork_value!=0){
            fork_value=fork();
            pid[a]=getpid();
            a++;
            n_kids--;
        }
        else
            break;
    }


    if(fork_value==0){
        execve("./char-loop",args,NULL);
    }

    sleep(1);
    while(a>=0){
        kill(pid[a],SIGINT);
        a--;
    }

    while(wait(&uscita)!=-1){
        s[cont]=uscita >> 8;
        cont++;
        printf("%c\n",s[cont-1]);
    }
    s[cont] = 0;
    printf("%s\n",s);
    //stampa(s,atoi(argv[1]));

    return 0;
}

void stampa(char *str,int lung){
    for(;lung>0;lung--){
        printf("%c\n",str[lung]);
    }
}