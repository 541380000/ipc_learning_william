//
// Created by xie on 2021/5/22.
//
#include "include_all.h"
#include <semaphore.h>
#include <sys/mman.h>
#define SHM_NAME "/shared_memory_posix"
using namespace std;
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int main(){
    int fd;
    pthread_mutex_t *mutex;
    void* ptr;
    int *cnt;

    struct stat s;

    fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, FILE_MODE);
    fstat(fd, &s);

    ftruncate(fd, sizeof(int) + sizeof(pthread_mutex_t));
    ptr = mmap(nullptr, sizeof(int) + sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    memset(ptr, 0, sizeof(int) + sizeof(pthread_mutex_t));
    close(fd);

    cnt = static_cast<int*>(ptr);
    mutex = static_cast<pthread_mutex_t*>((void*)((char*)ptr + 4));
    printf("addr: %ul %ul\n", cnt, mutex);

    // to share mutex between different process, you need to set pshared flag in attr
    pthread_mutexattr_t attr{};
    pthread_mutexattr_setpshared(&attr, 1);
    pthread_mutex_init(mutex, &attr);
    if (fork() == 0){
        for (int i=0; i<10000; i++){
            pthread_mutex_lock(mutex);
            (*cnt)++;
            printf("sub-> %d\n", *cnt);
            pthread_mutex_unlock(mutex);
        }
        exit(0);
    }

    for (int i=0; i<10000; i++){
        pthread_mutex_lock(mutex);
        (*cnt)++;
        printf("main-> %d\n", *cnt);
        pthread_mutex_unlock(mutex);
    }

    pthread_mutex_destroy(mutex);
}
