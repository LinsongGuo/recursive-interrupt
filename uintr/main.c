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

// create 10 file descriptors.
#define count 10
volatile unsigned long uintr_received = 0;
int descriptor[count];
int stui_flag;

// let the processor run for cycles.
void compute(int cycles) {
	unsigned long long c1 = __rdtsc(), c2 = c1;
	while (c2 - c1 <= cycles) {
		c2 = __rdtsc();
	}
}

void print_start(unsigned long long vector) {
	printf("handler %llu start\n", vector);
}

void print_end(unsigned long long vector) {
	printf("handler %llu end\n", vector);
}

void __attribute__ ((interrupt))
     __attribute__((target("general-regs-only", "inline-all-stringops")))
     ui_handler(struct __uintr_frame *ui_frame,
		unsigned long long vector) {
	
	print_start(vector);

	uintr_received++;
	if (stui_flag)
		_stui();
	compute(1000 * 1000 * 100); // make the handler run for a long time

	print_end(vector);
}

void *sender(void *arg) {
	int i;
	int uipi_index[count];
	for (i = 0; i < count; ++i) {	 
		uipi_index[i] = uintr_register_sender(descriptor[i], 0);
		if (uipi_index[i] < 0)
			perror("Sender register error\n");
	}

	for (i = 0; i < count; ++i) {
		// Send User IPI
		// printf("senduipi %d\n", uipi_index[i]);
		_senduipi(uipi_index[i]);
		
		compute(10000);
	}

	return NULL;
}

void send_uintrs() {
	pthread_t pt;
	// Create another thread
	if (pthread_create(&pt, NULL, &sender, NULL)) {
		perror("Error creating sender thread");
	}

	compute(1000 * 1000 * 2000);
	
	pthread_join(pt, NULL);
	
	printf("%d UIPIs sent, %ld received\n", count, uintr_received);
}

int main(int argc, char* argv[]) {
	stui_flag = argc > 1 && strcmp(argv[1], "stui") == 0 ? 1 : 0;

	// Register uintr handler.
	int handler = uintr_register_handler(ui_handler, 0);
	if (handler) {
		perror("Interrupt handler register error\n");
	}

	// Create new uintrfd file descriptors.
	int i;
	for (i = 0; i < count; ++i) {
		descriptor[i] = uintr_create_fd(i, 0);
		if (descriptor[i] < 0)
			perror("Interrupt vector allocation error\n");
	}

	// Enable interrupts
	_stui();

	send_uintrs();

	return 0;
}
