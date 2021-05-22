//
// Created by xie on 2021/5/22.
//
#include "include_all.h"
using namespace std;
int test4_main(){
    int res;
    pthread_rwlockattr_t attr{};
    // if you want to set the rwlock shared by process(normally it is), use attr
    // in experiment, it cannot be shared between process
    // we have to put it in a shared memory to shared it between processes
    res = pthread_rwlockattr_setpshared(&attr, 1);
    printf("set shared: %d\n", res);
    pthread_rwlock_t rwlock{};
    pthread_rwlock_init(&rwlock, &attr);

    if (fork() == 0){

        sleep(1);
        res = pthread_rwlock_trywrlock(&rwlock);
        printf("res2: %s\n", strerror(res));
        exit(0);
    }
    res = pthread_rwlock_rdlock(&rwlock);
    printf("res1: %d\n", res);
    sleep(2);
    res = pthread_rwlock_trywrlock(&rwlock);
    printf("%d: %s\n", res, strerror(res));
    pthread_rwlock_unlock(&rwlock);
    if (res != 0){
        pthread_rwlock_destroy(&rwlock);
        return -1;
    }

}
