// Selim Tanriverdi and William Kelley
// CS 3013 C02 2016

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>


#define MAX_BUFF_LEN 128 // Max chars for input buffer
#define MAX_TOKENS 32 // Max number of space delimited tokens from input buffer

// Special commands or characters
#define BGTOK "&"
#define CDTOK "cd"
#define EXITTOK "exit"
#define JOBSTOK "jobs"

// Struct for handling background processes, keeps a double linked list
// where the tail points to the head to never lose track of size.
typedef struct bgProc{
    int pid;
    int job;
    long start;
    char command[MAX_BUFF_LEN];
    struct bgProc* prev; // Previous process
    struct bgProc* next; // Next process
} bgProc;


int cnt = 0;

// We are going to keep a double linked list of pointers of typdef bgProc
// This will keep track of which process is a fork or background od which process
bgProc* bgProcHead = NULL;
bgProc* bgProcTail = NULL;
int bgProcNum=0;


/*
 * Gets process if it exists in list of background processes. Uses PID to find process.
 *
 * @param pid The process ID of the process being searched for.
 * @return bgProc* The process removed, or NULL if does not exist
 */
bgProc* getProc(int pid){
    bgProc* process = bgProcHead;
    while (process != NULL){
        if (process->pid == pid){
            return process;
        }
        process = process->next;
    }
    return NULL;
}

/*
 * Removes a processs with pid
 *
 * @param pid Process ID of process to be removed
 * @return bgProc* pointer to process being removed 
 * returns this process when removed
 */
bgProc* killProc(int pid){
    bgProc *process = getProc(pid);
    if (process == NULL){
        return NULL;
    }
    if (process->prev != NULL){
      process->prev->next = process->next;
    }
    if (process->next != NULL){
        process->next->prev = process->prev;
    }
    if (process == bgProcHead){
        bgProcHead = process->next;
    }
    if (process == bgProcTail){
        bgProcTail = process->prev;
    }
    bgProcNum--;
    return process;
}

/*
 * Prints Process Usage Info
 *
 * @param time_before The time from rusage statistics to be subtracted
 * to get time of current process only
 * @param pid Process ID of process to print Usage Statistics on
 * param pid Process
 */
void printInfo(long time_before, int pid, int exitStat){
    struct rusage resource_usage;
    struct timeval current_time; 
    gettimeofday(&current_time, NULL);
    long elapsed_time = (current_time.tv_sec * 1000) + (current_time.tv_usec/1000) - time_before;
    getrusage(RUSAGE_CHILDREN, &resource_usage);

    if (WEXITSTATUS(exitStat) == 0){
        printf("\n");
        printf("\n");
        printf("|PID : %4d               |\n", pid);
        printf("|Time Elapsed (mS): %4ld  |\n", elapsed_time);
        printf("|CPU Time (mS):           |\n");
        printf("|  -User (ms): %4ld       |\n", ((resource_usage.ru_utime.tv_sec * 1000) + (resource_usage.ru_utime.tv_usec / 1000)));
        printf("|  -System (mS): %4ld     |\n", ((resource_usage.ru_stime.tv_sec * 1000) + (resource_usage.ru_stime.tv_usec / 1000)));
        printf("|Preempted:               |\n");
        printf("|  -Volunatry: %4ld       |\n", resource_usage.ru_nivcsw);
        printf("|  -Involuntary: %4ld     |\n", resource_usage.ru_nvcsw);
        printf("|Page faults: %4ld        |\n", resource_usage.ru_majflt);
        printf("|Page Reclaims: %4ld      |\n", resource_usage.ru_minflt);
        printf("\n");
        printf("\n"); 
    }
}

int getNewJobNum(){
    int id = 0;
    bgProc* process = bgProcHead;
    while (process != NULL){
        if (process->job > id){
            id = process->job;
        }
    }
    return id++;
}

/*
 * Checks if BG Processes are running or complete
 */
void bgProcCheck(){
    int pid;
    int exitStat;

    while ((pid = waitpid(-1, &exitStat, WNOHANG)) > 0){
        bgProc* process = killProc(pid);
        if (process == NULL){
            fprintf(stderr, "Error while removing process.\n");
        }
        else{
            printf("Process [%s] is completed.\n", process->command);
            printInfo(process->start, pid, exitStat);
        }
    }
}

/*
 * Executes Commands in Background or Foregrounde depending on @param background
 *
 * @param commToExecute[] The command and its arguments to run.
 * @param background Run process in background or not.
 */
void run_command(char* commToExecute[], int background){
    int child_pid; // Process ID
    // Fork a new process for the command
    if ((child_pid = fork()) < 0){
        fprintf(stderr, "ERROR WHILE FORKING PROCESS \n");
        exit(1);
    }
    //Child Process
    else if (child_pid == 0){
        // Execute the command
        if (execvp(commToExecute[0], commToExecute) < 0){
            fprintf(stderr, "ERROR DURING EXEC \n");
            exit(1);
        }
    }
    // Parent process
    else{
        struct timeval start;
        gettimeofday(&start, NULL);
        long start_time = ((start.tv_sec * 1000) + (start.tv_usec/1000));
        // Foreground process
        if (background == 0){
            int exitStat;
            waitpid(child_pid, &exitStat, 0);
            printInfo(start_time, child_pid, exitStat);
        }
        // Background process
        else{
            bgProcNum++;
            bgProc* new_process = malloc(sizeof(bgProc));
            new_process->pid = child_pid;
            new_process->job = getNewJobNum();
            new_process->start = start_time;
            strcpy(new_process->command, commToExecute[0]);
			
			// No list exists
            if (bgProcTail == NULL){
                new_process->prev = NULL;
                new_process->next = NULL;
                bgProcTail = new_process;
                bgProcHead = new_process;
            }
            
            // Add to end of list
            else{
                new_process->prev = bgProcTail;
                new_process->next = NULL;
                bgProcTail->next = new_process;
                bgProcTail = new_process;
            }

            sleep(1);
            printf("Job : %d, PID : %d\n", new_process->job, new_process->pid);
        }
    }
}

char** split_args(char* buffer){
    char* clone = (char*)(malloc(128));
    int   count = 0;
    strncpy(clone, buffer, 128);

    char* toks = strtok (clone," ");
    while (toks != NULL)
    {
        count++;
        toks = strtok (NULL, " ");
    }
    free(toks);

    char** result = (char**)(malloc(sizeof(char*) * count));
    char** writer = result;

    toks = strtok (buffer," ");
    while (toks != NULL)
    {
        *(writer++) = toks;
        toks = strtok (NULL, " ");
    }
    free(toks);

    return result;
}

/*
 * Main function, user input for shell commands
 *
 * @param argc Number of arguments from command line which launched this program
 * @param argv[] Actual char array of arguments entered from command line
 * @return 0 if successful, 1 if failed
 */
int main(int argc, char *argv[]){
    // Run with given arguments, quit when complete
    if (argc > 1){
        int i =0;
        // char ** args = split_args(*argv);
        // printf("%s LOLOLOLOLOL\n",*args);
        // // run_command(args + 1, 0);
    }
    else{
        int exit_process = 0; // If exit has been requested
		// While process not done or exit not requested
        while (!exit_process) {
            char* input = NULL;
            size_t buflen = 0;
            ssize_t input_length = 0;

            printf("Enter command and arguments.\n$ ");
    
            input_length = getline(&input, &buflen, stdin);
            // Invalid input
            if (input_length == -1){
                break;
            }
            // Input buffer too long
            else if (input_length > MAX_BUFF_LEN) {
                printf("ERROR More than 128 chars in input buffer.\n");
            }
            else{
                char* totTok[MAX_TOKENS];
                char* token = NULL;
                int numTok = 0;
                
                bgProcCheck();
                // Split input by new line and spaces
                while ((token = strtok(input, " \n")) != NULL){
                    input = NULL;
                    if (numTok >= MAX_TOKENS){
                        printf("ERROR More than 32 argument tokens entered.\n");
                        break;
                    }

                    totTok[numTok] = token;
                    numTok++;
                }
                totTok[numTok] = NULL;

                if (numTok <= 0 || numTok >= MAX_TOKENS){
                    continue;
                }

                int background = !strcmp(totTok[numTok - 1], BGTOK); // Determine if last character is '&'

                if (background){// Push task to background
                    totTok[numTok - 1] = NULL;
                    numTok--;
                }
                // Enter
                if (strcmp(totTok[0], EXITTOK) == 0){
                    exit_process = 1;
                }
                // CD
                else if (strcmp(totTok[0], CDTOK) == 0) {
                    // If directory change successful
                    if (chdir(totTok[1]) == -1){
                        printf("Invalid directory.\n");
                    }
                }
                // Jobs
                else if (strcmp(totTok[0], JOBSTOK) == 0){
                    if (bgProcHead == NULL){
                        printf("No processes running in background.\n");
                    }
                    else{
                        bgProc* process = bgProcHead;
                        while (process != NULL){
                            printf("Job : %d, Process : %5d, Command : %s\n", process->job, process->pid, process->command);
                            process = NULL;
                        }
                    }
                }
                else{
                     run_command(totTok, background);
                }
            }
            input = NULL; // Reset input
        }
    }


    // Wait for all background processes
    if (bgProcNum > 0){
        printf("Waiting for background processes to complete...\n");
        while (bgProcNum > 0){
            bgProcCheck(); // Clear background tasks before exiting
        }
    }
    printf("EXIT Shell,\nGoodbye!\n");
    return 0;
}