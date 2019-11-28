#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/wait.h> 
#include <signal.h>
#include <unistd.h>
#include <strings.h>

unsigned char c;

void my_handler(int signal){
	if (signal == SIGINT)
	{
		//printf("hey,funziono\n");
		exit(c);
	}
}

int main(int argc, char const *argv[])
{
	c = argv[1][0];
	struct sigaction sa;

	bzero(&sa,sizeof(sa));
	sa.sa_handler = my_handler;
	sigaction(SIGINT, &sa, NULL);

	while(1){
		c++;
		if (c>126)
			c=33;
		//printf("%c\n",c);
	}

	return c;
}