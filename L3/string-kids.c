#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char  const *argv[])
{
	int status=0, n_child = atoi(argv[1]), f_value=-1, i=0;
	char *args[] = {"ls","Hello",NULL};
	char * string;
	pid_t * children;
	
	children = malloc(sizeof(*children) *  n_child);

	for ( i= 0; i < n_child && f_value!=0; ++i)
	{
		f_value=fork();
		children[i] = f_value;
	}

	if (f_value==0)
	{
		//execve("/home/lorenzo/workspaces/char-loop",args,NULL);
		execve("./char-loop",args,NULL);
		//system("./char-loop afds");
	}else{
		sleep(1);
		for (i = 0; i < n_child; ++i)
		{
			kill(children[i],SIGINT);
		}

		string = malloc(sizeof(char)*n_child);
		i=0;
		while (wait(&status)!=-1){
			string[i]=status >> 8;
			i++;
		}
		string[i]=0;
		printf("%s\n",string);
	}
	return 0;
}
