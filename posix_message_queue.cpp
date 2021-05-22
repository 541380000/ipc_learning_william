//
// Created by xie on 2021/5/21.
//
#include <mqueue.h>
#include <sys/wait.h>
#include "include_all.h"

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
/*
 * Message queues aren't really files. You don't create them under Real file system like /tmp
 * So the name here cannot be something like /home/xie/tmp, mq_open returns Permission denied in such occasions
 */
#define POSIX_MQ "/test.1234"

using namespace std;
int posix_message_queue_test(){
    int c, flags;
    mqd_t mqd;
    flags = O_RDWR | O_CREAT;
    mqd = mq_open(POSIX_MQ, flags, FILE_MODE, nullptr);
    if (mqd == -1){
        perror("mq_open");
        flags = O_RDWR;
        mqd = mq_open(POSIX_MQ, flags, FILE_MODE, nullptr);
    }
    mq_attr attr{};
    printf("%d %d\n", mqd, mq_getattr(mqd, &attr));
    printf("MQ Attr = {current message: %ld, flags: %ld, max messages: %ld, max size: %ld}\n", attr.mq_curmsgs, attr.mq_flags, attr.mq_maxmsg, attr.mq_msgsize);

    //attr.mq_flags |= O_NONBLOCK;
    //setattr is only used to set NonBlock flag, other options should be set when message queue is opened

    char msg[10240+1] = "Hello This is the message";
    mq_send(mqd, msg, strlen(msg), 0);
    pid_t childpid = 0;
    if ((childpid = fork()) == 0){
        uint32_t prio = 0;
        int res = mq_receive(mqd, msg, attr.mq_msgsize, &prio);
        if (res < 0){
            perror("mq_receive");
            return -1;
        }
        msg[res] = '\0';
        printf("Received in child porcess: %s\n", msg);
        exit(0);
    }
    waitpid(childpid, nullptr, 0);
    mq_close(mqd);
    mq_unlink(POSIX_MQ);
}
