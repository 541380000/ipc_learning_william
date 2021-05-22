//
// Created by xie on 2021/5/22.
//

#include <semaphore.h>
#include "include_all.h"
using namespace std;
#define SEM_NAME "/test_sem1234"
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
int test6_main(){
    sem_t *sem;
    unsigned int val = 1;
    sem = sem_open(SEM_NAME, O_RDWR | O_CREAT, FILE_MODE, val);
    printf("%u\n", sem);
    int curval = 0;
    sem_getvalue(sem, &curval);
    printf("current val: %d\n", val);

    // you can block with sem_wait, or unblock with sem_trywait
    // they return 0 if success else not 0
    if (fork() == 0){   //child process
        for (int i=0; i<1000; i++){
            sem_wait(sem);
            sem_getvalue(sem,&curval);
            printf("%d====\n", curval);
            sem_post(sem);
        }
        exit(0);
    }

    for (int i=0; i<1000; i++) {
        if (sem_trywait(sem) != 0){
            i--;
            continue;
        }
        sem_getvalue(sem,&curval);
        printf("%d----\n", curval);
        sem_post(sem);
    }

    sem_close(sem);
    sem_unlink(SEM_NAME);

}
