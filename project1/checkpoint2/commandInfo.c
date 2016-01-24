// Selim Tanriverdi and William Kelley
// CS 3013 C02 2016

#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 
#include <sys/time.h>
#include "commandInfo.h"


usage_info* get_init(){
	usage_info* init = (usage_info*) (malloc(sizeof(struct usage_info)));
	struct timeval t;
	gettimeofday(&t, 0);
	init->wallClockTime = t.tv_sec*1000 + t.tv_usec/1000;
	return init;
}

usage_info** get_usage_info(struct rusage *usage, usage_info *shell){
	//init struct data
	usage_info* exec = (usage_info*) (malloc(sizeof(struct usage_info)));
	struct timeval t;
	gettimeofday(&t, 0);
	long elapsed = t.tv_sec*1000 + t.tv_usec/1000;

	//write the stastics
	exec -> count_preempted = 0
			- shell -> count_preempted + 
			usage -> ru_nivcsw;
	exec -> countInvoluntary = 0
			- shell -> countInvoluntary +
			usage -> ru_nvcsw;
	exec -> countPageFaults = 0
			- shell -> countPageFaults +
			usage -> ru_majflt;
	exec -> countPageReclaims = 0
			- shell -> countPageReclaims +
			usage -> ru_minflt;
	exec -> systemCPUTime = 0
			- shell -> systemCPUTime +
			usage -> ru_stime.tv_sec*1000 +
			usage -> ru_stime.tv_usec/1000;
	exec -> userCPUTime = 0
			- shell -> userCPUTime +
			usage -> ru_utime.tv_sec*1000 +
			usage -> ru_utime.tv_usec/1000;
	exec -> wallClockTime = 
			elapsed - 
			shell -> wallClockTime;

	//overwrite the status numbers
	shell -> count_preempted = 
			usage -> ru_nivcsw;
	shell -> countInvoluntary = 
			usage -> ru_nvcsw;
	shell -> countPageFaults = 
			usage -> ru_majflt;
	shell -> countPageReclaims = 
			usage -> ru_minflt;
	shell -> systemCPUTime = 
			usage -> ru_stime.tv_sec*1000 +
			usage -> ru_stime.tv_usec/1000;
	shell -> userCPUTime =
			usage -> ru_utime.tv_sec*1000 +
			usage -> ru_utime.tv_usec/1000;
	shell -> wallClockTime = elapsed;

	usage_info** result = (usage_info**)(malloc(2* sizeof(usage_info*)));
	*result = shell;
	*(result+1) = exec;

	return result;
}



void print_info(usage_info* p){
	printf("\n");
	printf("\n");
	printf("Wall Clock Time Elapsed (mS): %i\n", p->wallClockTime);	// Wall Clock Time
	printf("CPU Time (mS):\n");
	printf("    User:         %i\t\n", p->userCPUTime);
	printf("    System:       %i\n", p->systemCPUTime);
	printf("Preemption: \n");
	printf("    Involuntary:   %i\n", p->countInvoluntary);
	printf("    Voluntary:    %i\n", p->count_preempted);
	printf("Page Faults:      %i\n", p->countPageFaults);
	printf("Page Reclaims:    %i\n", p->countPageReclaims);
	printf("\n");
	printf("\n");	
}