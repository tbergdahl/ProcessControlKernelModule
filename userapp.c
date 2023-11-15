#include "userapp.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

// Additional headers as needed


// Register process with kernel module
void register_process(unsigned int pid)
{
    char command [200];
    memset(command, '\0', 200);
    sprintf(command, "echo %u > /proc/kmlab/status", pid);
    system(command);
}

int main(int argc, char* argv[])
{
    int __expire = 10;
    time_t start_time = time(NULL);

    if (argc == 2) 
    {
        __expire = atoi(argv[1]);
    }

    register_process(getpid());

    while (1) 
    {
        if ((int)(time(NULL) - start_time) > __expire) 
        {
            break;
        }
    }

	return 0;
}
