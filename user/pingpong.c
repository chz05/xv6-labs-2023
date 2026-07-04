#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int
main(int argc, char *argv[])
{
    if(argc != 1){
        fprintf(2, "usage: pingpong\n");
        exit(1);
    }
    int p1[2], p2[2];
    char* byte = "a";

    if (pipe(p1) < 0) {
        fprintf(2, "pingpong: pipe1 failed\n");
        exit(1);
    }
    if (pipe(p2) < 0) {
        fprintf(2, "pingpong: pipe2 failed\n");
        exit(1);
    }

    if (fork() == 0) {
        // child process
        read(p1[0], &byte, 1);
        printf("%d: received ping\n", getpid());
        write(p2[1], &byte, 1);
        close(p2[1]);
        close(p1[0]);
        
    }
    else {
        //parent process
        write(p1[1], &byte, 1);
        read(p2[0], &byte, 1);
        printf("%d: received pong\n", getpid());        
        close(p2[0]);
        close(p1[1]);
        wait(0);
    }
    exit(0);
}