//
// Created by xie on 2021/5/21.
//
#include "include_all.h"
#include <csignal>
#include <cstdlib>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;
void signal_rt_handler(int signo, siginfo_t* siginfo, void* ptr);
typedef void (*RT_SIG_HANDLER)(int, siginfo_t*, void*);
RT_SIG_HANDLER signal_rt(int signo, void (*func)(int, siginfo_t*, void*), sigset_t *mask){
    struct sigaction act, oact;
    act.sa_sigaction = func;
    act.sa_flags = SA_SIGINFO;
    act.sa_mask = *mask;
    if (sigaction(signo, &act, &oact) < 0){
        perror("sigaction");
        return nullptr;
    }
    return oact.sa_sigaction;
}


int test3_main(){
    pid_t pid;
    sigset_t newmask{}, oldmask{}, zeromask{};
    sigval val;
    printf("MAX realtime: %d, MIN realtime: %d\n", (int)SIGRTMAX, (int)SIGRTMIN);
    if ((pid = fork()) == 0){
        printf("%d, Child sending!\n", getpid());
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGRTMAX);
        sigaddset(&newmask, SIGRTMAX - 1);
        sigaddset(&newmask, SIGRTMAX - 2);
        sigprocmask(SIG_BLOCK, &newmask, &oldmask);
        signal_rt(SIGRTMAX, signal_rt_handler, &newmask);
        signal_rt(SIGRTMAX - 1, signal_rt_handler, &newmask);
        signal_rt(SIGRTMAX - 2, signal_rt_handler, &newmask);
        sleep(2);
        sigprocmask(SIG_UNBLOCK, &newmask, nullptr);
        sleep(1);
        exit(0);
    }
    sleep(1);
    printf("%ld\n", pid);
    for (auto i = SIGRTMAX; i>=SIGRTMAX-2; i--){
        for (int j=0; j<3; j++){
            val.sival_int = j;
            sigqueue(pid, i, val);
        }
    }
    waitpid(pid, nullptr, 0);
}


void signal_rt_handler(int signo, siginfo_t* siginfo, void* ptr){
    printf("signo: %d, value is %d\n", signo, siginfo->si_value);
}
