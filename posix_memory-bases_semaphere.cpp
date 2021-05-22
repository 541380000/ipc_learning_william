//
// Created by xie on 2021/5/22.
//
/*
 * semaphore based on memory is more efficient because it doesn't concern system call
 */
#include <semaphore.h>
#include "include_all.h"
using namespace std;
static sem_t sem;
void* func(void* ptr){
    printf("Thread: %ld starting", gettid());
    for (int i=0; i<1000; i++){
        sem_wait(&sem);
        int val;
        sem_getvalue(&sem, &val);
        printf("%d: %d====\n", gettid(), val);
        sem_post(&sem);
    }
    return 0;
}

int test7_main(){
    pthread_t pthread{};
    pthread_create(&pthread, nullptr, func, nullptr);
    sem_init(&sem, 0, 1);

    for (int i=0; i<1000; i++){
        sem_wait(&sem);
        int val;
        sem_getvalue(&sem, &val);
        printf("%d: %d----\n", gettid(), val);
        sem_post(&sem);
    }

    sem_destroy(&sem);

}
