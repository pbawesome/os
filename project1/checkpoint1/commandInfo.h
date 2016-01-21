// Selim Tanriverdi and William Kelley
// CS 3013 C02 2016


#include <sys/resource.h>

typedef struct usage_info {
   int wallClockTime;		// Elapsed Wall Clock Time
   int userCPUTime;			// User CPU Time
   int systemCPUTime;		// System CPU Time
   int count_preempted;		// Involuntary Preempted
   int countInvoluntary;   	// Voluntary Prempted
   int countPageFaults;  	// Page Faults
   int countPageReclaims;   // Page Reclaims
} usage_info;

usage_info*  get_init();
usage_info** get_usage_info(struct rusage *usage, usage_info *shell);
void print_info(usage_info* p);