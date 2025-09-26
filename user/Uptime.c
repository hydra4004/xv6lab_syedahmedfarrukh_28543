#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int ticks_elapsed = uptime();
    printf("System has been running for %d ticks.\n", ticks_elapsed);
    exit(0);
}
