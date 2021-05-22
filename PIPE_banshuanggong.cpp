#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
using namespace std;
#define SIZE_1MB 1024

void client(int reader, int writer){
    char buffer[SIZE_1MB + 1];
    printf("Client-> Please input file path:\r\n");
    fgets(buffer, SIZE_1MB, stdin);
    sleep(1);
    int len = strlen(buffer);
    if (buffer[len-1] == '\n')
        buffer[len-1] = '\0', len--;
    int res = write(writer, buffer, len);
    printf("Client-> Writing %s to pipe\r\n", buffer);
    return;
}

void server(int reader, int writer){
    char buffer[SIZE_1MB + 1];
    int res = read(reader, buffer, SIZE_1MB);
    if (res == 0)
        perror("End of file when reading from pipe\r\n");
    buffer[res] = '\0';
    printf("Server-> Reading from pipe: %s\r\n", buffer);
    return;
}

int banshuanggong() {
    int pipe_fd1[2], pipe_fd2[2];
    pid_t childpid;
    if (pipe(pipe_fd1) != 0 || pipe(pipe_fd2) != 0)
        perror("Error creating pipe\r\n");
    if ((childpid = fork()) == 0){  //child
        close(pipe_fd1[1]);
        close(pipe_fd2[0]);
        server(pipe_fd1[0], pipe_fd2[1]);
        exit(0);
    }
    close(pipe_fd1[0]);
    close(pipe_fd2[1]);
    client(pipe_fd2[0], pipe_fd1[1]);

    exit(0);
}
