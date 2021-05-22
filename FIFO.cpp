/*
 * Atomic is garenteed when message size is less than a defined size
 *
 */

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
using namespace std;
// Allow Read and Write by user process, Read by same group and other
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define FIFO1 "/home/xie/william_fifo1"
#define FIFO2 "/home/xie/william_fifo2"

int FIFO_TEST(){
    int read_fd, write_fd;
    int childpid;
    if (mkfifo(FIFO1, FILE_MODE) < 0 && errno != EEXIST)
        perror("mkfifo"), exit(-1);
    if (mkfifo(FIFO2, FILE_MODE) < 0 && errno != EEXIST)
        unlink(FIFO1), perror("mkfifo"), exit(-1);
    if ((childpid = fork()) == 0){ //child process
        read_fd = open(FIFO1, O_RDONLY, 0);
        write_fd = open(FIFO2, O_WRONLY, 0);
        char buffer[1024+1];
        int res = read(read_fd, buffer, 1024);
        if (res < 0)
            perror("read"), exit(-1);
        buffer[res] = '\0';
        printf("Server-> Received from client: %s\n", buffer);
        char reply[1024+1];
        sprintf(reply, "I get message from you: %s", buffer);
        write(write_fd, reply, strlen(reply));
        exit(0);
    }
    // when open with RDONLY is called, it will block until open with WRONLY is called
    // so you have to open FIFO1 here first
    write_fd = open(FIFO1, O_WRONLY, 0);
    read_fd = open(FIFO2, O_RDONLY, 0);

    char buffer[1024 + 1] = "This is the message!";
    write(write_fd, buffer, strlen(buffer));
    int res = read(read_fd, buffer, 1024);
    if (res < 0)
        perror("read"), exit(-2);
    unlink(FIFO1);
    unlink(FIFO2);
    printf("Client-> received: %s\r\n", buffer);
}
