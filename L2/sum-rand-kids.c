#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#define NUM_KIDS 20

int main(int argc, char const *argv[])
{
	int n_fork=-1, total =0, status=0;
	pid_t pid;

	setvbuf(stdout,NULL,_IONBF,0);
	printf("%d\n",NUM_KIDS );

	for (int i = 0; i < NUM_KIDS; ++i)
	{
		if (n_fork!=0)
			n_fork=fork();
		else
			break;
	}
	if (n_fork==0)
	{
		pid=getpid();
		srand(pid);
		int randomn_n = (rand() % 6) +1;
		printf("pid %d, randomn_n %d\n", pid,randomn_n);
		return randomn_n;
	}else{
		while(wait(&status)!=-1){
			total += status >> 8;
		}

		printf("total %d\n",total);
	}
	return 0;
}