#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    if(argc < 4){
        printf("Usage: sandbox mask path command...\n");
        exit(1);
    }

    int mask = atoi(argv[1]);
    char *path = argv[2];

    if(fork() == 0){
        interpose(mask, path);
        exec(argv[3], &argv[3]);
        printf("exec failed\n");
        exit(1);
    } else {
        wait(0);
    }
    exit(0);
}
