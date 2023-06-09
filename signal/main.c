#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <x86intrin.h>

void compute(int cycles) {
	unsigned long long c1 = __rdtsc(), c2 = c1;
	while (c2 - c1 <= cycles) {
		c2 = __rdtsc();
	}
}

void signal_unblock(void)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

int unblock_flag; // The flag to indicate if unblock signals in the signal handler.
void signal_handler(int signo) {
    printf("signal handler starts\n");
   
    if (unblock_flag) {
        signal_unblock();
    }
    compute(1000 * 1000 * 2000);
    printf("signal handler ends\n");
}

void sig_init() {
    struct sigaction psa;
    psa.sa_handler = signal_handler;
    sigaction(SIGUSR1, &psa, NULL);
}

int timer_init() {
    timer_t timerID;
    struct sigevent sev;
    struct itimerspec its;

    // Create the timer
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGUSR1;
    sev.sigev_value.sival_ptr = NULL;
    sev.sigev_notify_attributes = NULL;

    if (timer_create(CLOCK_REALTIME, &sev, &timerID) == -1) {
        perror("timer_create");
        exit(EXIT_FAILURE);
    }

    // Set the timer expiration and interval: 1000ns
    int interval = 1000;
    its.it_value.tv_sec = 0; 
    its.it_value.tv_nsec = interval;
    its.it_interval.tv_sec = 0; 
    its.it_interval.tv_nsec = interval;

    // Start the timer
    if (timer_settime(timerID, 0, &its, NULL) == -1) {
        perror("timer_settime");
        exit(EXIT_FAILURE);
    }

    // compute(10 * 1000);
    // printf("stop\n");
    // if (timer_delete(timerID) == -1) {
    //     perror("timer_delete");
    //     exit(EXIT_FAILURE);
    // }

    // while(1);
}

int main(int argc, char* argv[]) {
    unblock_flag = argc > 1 && strcmp(argv[1], "unblock") == 0 ? 1 : 0;

    // printf("flag: %d\n", unblock_flag);

    sig_init();

    timer_init();

    return 0;
}