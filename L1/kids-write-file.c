#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    FILE * fptr;
    int n_kids=0, n_writes=0;
    int fork_value=-1;
    pid_t pid,ppid;

    if (argc != 4) {
        printf("wrong arguments number\n");
        exit(1);
    }
    if ((fptr=fopen(argv[1],"w"))==NULL){
        printf("file not found\n");
        exit(1);
    }
    n_kids=atoi(argv[2]);
    n_writes=atoi(argv[3]);

    int i=0;
    //creazione figlio
    while (fork_value!=0 && i<n_kids){
        ++i;
        fork_value=fork();
    }

    if (fork_value==0){
        pid=getpid();
        ppid=getppid();
        for (int i = 0; i <n_writes ; ++i) {
            fprintf(fptr,"%d %d\n", pid, ppid);
        }
    }

    while (wait(NULL) != -1);

    fclose(fptr);
    return 0;
}
