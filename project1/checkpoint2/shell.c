// Selim Tanriverdi and William Kelley
// CS 3013 C02 2016

#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include "commandInfo.h"

usage_info** execute(char** exec, usage_info* status)
{
	struct rusage *r = (struct rusage*)(malloc(sizeof(struct rusage)));

	int pid = fork();
	inject_time(status);
	if (pid == 0){ // Child Process
		execvp(exec[0], exec);
	}
	else{ // Parent Process
		wait(pid);
		getrusage(RUSAGE_SELF, r);
		return get_usage_info(r, status);
	}
}


void inject_time(usage_info* inf){
	struct timeval t;
	gettimeofday(&t, 0);
	inf->wallClockTime = (long long)(t.tv_sec*1000 + t.tv_usec/1000);
}

char** splitArgs(char* buffer){
	char* clone = (char*)(malloc(128));
	int   cnt = 0;
	strncpy(clone, buffer, 128);

	char* tokens = strtok (clone," ");
	while (tokens != NULL){
		cnt++;
		tokens = strtok (NULL, " ");
	}
	free(tokens);

	char** result = (char**)(malloc(sizeof(char*) * cnt));
	char** writer = result;

	tokens = strtok (buffer," ");
	while (tokens != NULL){
		*(writer++) = tokens;
		tokens = strtok (NULL, " ");
	}
	free(tokens);

	return result;
}

int change_dir(char* dirname){
	char dir[1024];
	if (dirname)	{
		chdir(dirname);
	}
	else{
		chdir("/");
	}
	if (getcwd(dir, sizeof(dir)) != NULL){
		printf("%s\n", dir);
	}
}

int main(int argc, char*argv[]){
	printf("~~$ ");
	char* buffer = (char*)malloc(128);
	int bufferspace = 0;
	int exiting = 0;
	char c = 0;
	usage_info *p = get_init();

	while(exiting == 0)	{
		c = getchar();
		if (c == '\n'){
			if (bufferspace != 0){
                char** args = splitArgs(buffer);

			    if (strcmp("exit", args[0]) == 0){
			    	exiting = 1;
			    }
			    else if (strcmp("cd", args[0]) == 0){
			    	change_dir(args[1]);
			    	printf("~~$");
		    	}
		    	else{
		    		usage_info **p2 = execute(args, p);
		    		p = p2[0];
		    		print_info(p2[1]);
		    		free(p2);
		    	}

		    	memset(buffer, 0, 128);
			    bufferspace = 0;
            }
            else{
                printf("~~$");
            }
        }
        else if (c == EOF){
            exiting = 1;
            printf("\n");
        }
		else{
			buffer[bufferspace++] = c;
		}
	}

	return 0;
}