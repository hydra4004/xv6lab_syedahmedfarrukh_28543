#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]) 
{
    if (argc != 2) {
        fprintf(2, "Usage: sleep <number_of_ticks>\n");
        exit(1);
    }

    int duration = atoi(argv[1]);
    if (duration < 1) {
        fprintf(2, "Error: argument must be a positive integer\n");
        exit(1);
    }

    // put the process to sleep for the given duration
    pause(duration);

    exit(0);
}
