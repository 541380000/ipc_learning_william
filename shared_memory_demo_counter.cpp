//
// Created by xie on 2021/5/22.
//
#include "include_all.h"
#include <sys/mman.h>
#include <semaphore.h>
using namespace std;
#define SHARED_MEMORY_FILE "/tmp/shared_memory"
#define SEM_NAME "/semaphore.1234"
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
int test8_main(){
    int fd = open(SHARED_MEMORY_FILE, O_RDWR | O_CREAT, FILE_MODE);
    int zero = 0;
    write(fd, &zero, sizeof(int));
    void* addr;
    int *cnt;
    addr = mmap(nullptr, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    cnt = static_cast<int*>(addr);
    close(fd);
    sem_t *sem;
    sem = sem_open(SEM_NAME, O_RDWR | O_CREAT, FILE_MODE, 1);
    // Better unlink semaphore just after open it
    // so that if error occurred in critical code, sem will be deleted from system after close is called automatically
    sem_unlink(SEM_NAME);
    setbuf(stdout, nullptr);
    int val;
    sem_getvalue(sem, &val);
    printf("init val of sem: %d\n", val);
    if (fork() == 0){
        for (int i=0; i<100; i++) {
            sem_wait(sem);
            (*cnt)++;
            printf("sub-> %d\n", *cnt);
            sem_post(sem);
        }
        exit(0);
    }

    for (int i=0; i<100; i++) {
        sem_wait(sem);
        (*cnt)++;
        printf("sub-> %d\n", *cnt);
        sem_post(sem);
    }
    sleep(1);
    sem_close(sem);
}
