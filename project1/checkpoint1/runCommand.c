// Selim Tanriverdi and William Kelley
// CS 3013 C02 2016

#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "commandInfo.h"

usage_info** executeCommand(char** exec, usage_info* status);

int main(int argc, char** argv){

	usage_info *ptr = get_init();
	usage_info **ptr2 = executeCommand(argv+1, ptr);
	ptr = ptr2[0];

	print_info(ptr2[1]);
}

usage_info** executeCommand(char** exec, usage_info* status){
	struct rusage *r = (struct rusage*)(malloc(sizeof(struct rusage)));
	int pid = fork();
	if (pid == 0){ // this is the child process
		int err = 0;
		err = execvp(exec[0], exec);
		if(err != 0){
			printf("ERROR! ILLEGAL COMMAND ENTERED!\n");
		}
	}
	else{ // this is the parent process
		wait(pid);
		getrusage(RUSAGE_SELF, r);
		return get_usage_info(r, status);
	}
}