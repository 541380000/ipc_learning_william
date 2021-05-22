/*
 * This code doesn't run property on Ubuntu 20.04 LTS.
 */

#include <unistd.h>
#include <string.h>
#include <iostream>
using namespace std;
int failed_Quanshuanggong(){
    int pipe_fd[2];
    if (pipe(pipe_fd) != 0) {
        perror("Error opening pipe");
        return -1;
    }
    pid_t childpid;
    if ((childpid = fork()) == 0){  //child process
        char buffer[1024 + 1];
        int res = read(pipe_fd[0], buffer, 1024);
        if (res < 0){
            perror("Error reading from pipe!");
            return -5;
        }
        buffer[res] = '\0';
        printf("Server-> Read: %s\n", buffer);
        buffer[res] = '\0';
        char reply[1024+1];
        sprintf("Received %s from you!", reply, buffer);
        printf("%s\n", reply);
        printf("%d", write(pipe_fd[0], reply, strlen(reply)));
        exit(0);
    }
    char buffer[1024 + 1] = "This is the message!";
    if (write(pipe_fd[1], buffer, strlen(buffer)) == -1){
        perror("Error writing!");
        return -2;
    }
    sleep(2);
    int res = read(pipe_fd[1], buffer, 1024);
    if (res < 0){
        perror("Error reading from pipe!");
        return -3;
    }
    printf("Client-> Received %s from server!\r\n");
}
