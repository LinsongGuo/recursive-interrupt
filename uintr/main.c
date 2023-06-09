#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <x86intrin.h>
#include <string.h>

#define __USE_GNU
#include <pthread.h>
#include <sched.h>

#ifndef __NR_uintr_register_handler
#define __NR_uintr_register_handler	471
#define __NR_uintr_unregister_handler	472
#define __NR_uintr_create_fd		473
#define __NR_uintr_register_sender	474
#define __NR_uintr_unregister_sender	475
#define __NR_uintr_wait			476
#endif

#define uintr_register_handler(handler, flags)	syscall(__NR_uintr_register_handler, handler, flags)
#define uintr_unregister_handler(flags)		syscall(__NR_uintr_unregister_handler, flags)
#define uintr_create_fd(vector, flags)		syscall(__NR_uintr_create_fd, vector, flags)
#define uintr_register_sender(fd, flags)	syscall(__NR_uintr_register_sender, fd, flags)
#define uintr_unregister_sender(ipi_idx, flags)	syscall(__NR_uintr_unregister_sender, ipi_idx, flags)
#define uintr_wait(flags)			syscall(__NR_uintr_wait, flags)

volatile unsigned long uintr_received = 0;
int descriptor;

void compute(int cycles) {
	unsigned long long c1 = __rdtsc(), c2 = c1;
	while (c2 - c1 <= cycles) {
		c2 = __rdtsc();
	}
}

void __attribute__ ((interrupt))
     __attribute__((target("general-regs-only", "inline-all-stringops")))
     ui_handler(struct __uintr_frame *ui_frame,
		unsigned long long vector) {

	uintr_received++;
	compute(2000 * 1000);
}

void *sender(void *arg) {
	
	int uipi_index = uintr_register_sender(descriptor, 0);
	if (uipi_index < 0)
		perror("Sender register error\n");

	int i, count = *((int*) arg);
	for (i = 0; i < count; ++i) {
		// Send User IPI
		_senduipi(uipi_index);
		
        // printf("senduipi, %ld\n", uintr_received);
		
		compute(2000);
	}

	return NULL;
}

void send_uintrs() {
    int count = 10;

	pthread_t pt;
	// Create another thread
	if (pthread_create(&pt, NULL, &sender, &count)) {
		perror("Error creating sender thread");
	}

	compute(1000 * 1000 * 1000);
	
	pthread_join(pt, NULL);
	
	printf("%d UIPIs sent, %ld received\n", count, uintr_received);

	close(descriptor);
}

int main(int argc, char* argv[]) {
	// Register uintr handler.
	int handler = uintr_register_handler(ui_handler, 0);
	if (handler) {
		perror("Interrupt handler register error\n");
	}

	// Create a new uintrfd file descriptor.
	descriptor = uintr_create_fd(0, 0);
	if (descriptor < 0)
		perror("Interrupt vector allocation error\n");

	// Enable interrupts
	_stui();

	send_uintrs();

	return 0;
}