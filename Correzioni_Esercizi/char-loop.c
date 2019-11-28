#include <stdio.h>
#define _GNU_SOURCE    /* NECESSARY from now on */
#include <unistd.h>
#include <string.h>
#include <signal.h> /* per usare sigaction(), sig*() */
#include <stdlib.h>
#include <sys/types.h>

unsigned char c;

void my_handler(int signum);

int main (int args, char *argv[]){

	struct sigaction sa;

	bzero(&sa,sizeof(sa));

	c=argv[1][0];

	sa.sa_handler=my_handler;
	sigaction(SIGINT,&sa,NULL);

	for(;;c++){
		if(c>126)
			c=33;	
	}
	

	return 0;	
}


void my_handler(int signum) {
	printf("Could handle signal %d (\"%s\") here, c=%d\n", signum, strsignal(signum),c);
	exit(c);	
}
