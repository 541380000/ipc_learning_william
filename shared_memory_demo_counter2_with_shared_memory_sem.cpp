//
// Created by xie on 2021/5/22.
//
#include "include_all.h"
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/wait.h>

using namespace std;
#define SHARED_MEMORY_FILE "/tmp/shared_memory"
#define SEM_NAME "/semaphore.1234"
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
int test9_main(){
    int fd = open(SHARED_MEMORY_FILE, O_RDWR | O_CREAT, FILE_MODE);
    int zero = 0;
    char* buffer = (char*)malloc(sizeof(int) + sizeof(sem_t));
    printf("size: %ld\n", sizeof(buffer));
    write(fd, buffer, sizeof(buffer));
    free(buffer);
    void* addr;
    int *cnt;
    addr = mmap(nullptr, sizeof(int) + sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    cnt = static_cast<int*>(addr);
    close(fd);
    auto *sem = static_cast<sem_t*>((void*)((char*)addr + 4));
    printf("%lu, %lu\n", cnt, sem);
    // if you want to share sem between processed, pshared should be 1
    sem_init(sem, 1, 1);
    setbuf(stdout, nullptr);
    int val;
    sem_getvalue(sem, &val);
    printf("init val of sem: %d\n", val);
    int pid;
    if ((pid = fork()) == 0){
        printf("String sub process!\n");
        for (int i=0; i<100; i++) {
            printf("Waiting!\n");
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
        printf("main-> %d\n", *cnt);
        sem_post(sem);
    }
    sleep(1);
    sem_getvalue(sem, &val);
    printf("lastval-> %d\n", val);
    waitpid(pid, 0, 0);
    sem_destroy(sem);
}
