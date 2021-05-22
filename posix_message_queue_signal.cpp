//
// Created by xie on 2021/5/21.
//
#include <mqueue.h>
#include <csignal>
#include "include_all.h"
using namespace std;
mqd_t mqd;
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define MQ_NAME "/test.12345"

sigevent sigev{};

void sig_user1(int no){

    mq_notify(mqd, &sigev);

    char buffer[10240];
    int cnt = mq_receive(mqd, buffer, 10240, nullptr);
    if (cnt < 0){
        perror("mq_receive");
        return ;
    }
    buffer[cnt] = '\n';
    buffer[cnt+1] = '\0';
    // calling printf in async handler is not safe
    // there a set of functions that is safe to call here, check the table in page 68 for detail
    write(1, "In signal handler: ", strlen("In signal handler: "));
    write(1, buffer, cnt+1);
    return;
}
int test_main() {

    mqd = mq_open(MQ_NAME, O_RDWR | O_CREAT, FILE_MODE, nullptr);
    if (mqd < 0) {
        perror("mq_open");
        return -1;
    }
    mq_attr attr{};
    mq_getattr(mqd, &attr);
    while(attr.mq_curmsgs != 0){
        char buffer[10240];
        uint32_t prio = 0;
        int cnt = mq_receive(mqd, buffer, 10240, nullptr);
        if (cnt == -1){
            perror("mq_receive");
            return -3;
        }
        printf("%d: %s\n", cnt, buffer);
        int res = mq_getattr(mqd, &attr);
        if (res != 0){
            perror("mq_getattr");
            return -2;
        }
        printf("%d\n", attr.mq_curmsgs);
    }
    printf("Finished cleaning message queue!\n");
    signal(SIGUSR1, sig_user1);
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGUSR1;
    mq_notify(mqd, &sigev);

    if (fork() == 0){
        for (int i=0; i<10; i++) {
            char *buffer = (char *) malloc(attr.mq_msgsize);
            strcpy(buffer, "This is the message");
            int res = mq_send(mqd, buffer, strlen(buffer), 0);
            if (res < 0) {
                perror("mq_send");
                return -5;
            }
            sleep(1);
        }
        exit(0);
    }
    mq_getattr(mqd, &attr);
    for (;;)
        pause();
    mq_close(mqd);
    unlink(MQ_NAME);
    exit(0);
}
