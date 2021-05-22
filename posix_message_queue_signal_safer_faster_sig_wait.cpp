//
// Created by xie on 2021/5/21.
//
#include <mqueue.h>
#include <csignal>
#include "include_all.h"
using namespace std;
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define MQ_NAME "/test.12345"

int test_main2() {
    sigevent sigev{};
    mqd_t mqd;

    mqd = mq_open(MQ_NAME, O_RDWR | O_CREAT | O_NONBLOCK, FILE_MODE, nullptr);
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
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGUSR1;
    mq_notify(mqd, &sigev);

    sigset_t zeromask{}, newmask{}, oldmask{};
    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigemptyset(&oldmask);

    sigaddset(&newmask, SIGUSR1);
    // You can also set mq_receive to UNBLOCK mode to read continiously from message queue
    while(1){
        char buffer[10240];
        int sig = -1;
        sigwait(&newmask, &sig);
        mq_notify(mqd, &sigev);
        if (sig == SIGUSR1){
            int cnt;
            while ((cnt = mq_receive(mqd, buffer, 10240, nullptr)) >= 0) {
                printf("Received: %s (%d bytes)\n", buffer, strlen(buffer));
            }
        }
    }
    mq_close(mqd);
    unlink(MQ_NAME);
    exit(0);
}
