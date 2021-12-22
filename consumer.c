/** Compilation: gcc -o memreader memreader.c -lrt -lpthread **/
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include "shmem.h"

static void* consumer(struct solverShmem *shmem)
{
	char str[100];
	int q_cons = shmem->cons;
	int q_prod = shmem->prod;
	struct Question *q;
	//sleep(5);
	
	if (q_prod == q_cons) {
		return;
	}
	printf("consumer: Producer %d Consumer %d\n", q_prod, q_cons);
	q = &shmem->array[q_cons];
	switch (q->op) {
	case PLUS:
		sprintf(str, "Result of %d + %d = %d\n", q->num1, q->num2, q->num1 + q->num2);
		break;
	case MIN:
		sprintf(str, "Result of %d - %d = %d\n", q->num1, q->num2, q->num1 - q->num2);
		break;
	case MUL:
		sprintf(str, "Result of %d * %d = %d\n", q->num1, q->num2, q->num1 * q->num2);
		break;
	case DIV:
		if (q->num2 == 0)
			sprintf(str, "Cannot divide by zero\n");
		else
			sprintf(str, "Result of %d / %d = %d\n", q->num1, q->num2, q->num1 / q->num2);
		break;
	default:
		sprintf(str, "Error: unknown op %d\n", q->op);
		break;
	}
	q_cons = (q_cons + 1) % Q_SIZE;
	puts(str);
	shmem->cons = q_cons;
}

int cons_main()
{
	int fd = shm_open(BackingFile, O_RDWR, AccessPerms);  /* empty to begin */
	if (fd < 0)
		report_and_exit("Can't get file descriptor...");

	/* get a pointer to memory */
	struct solverShmem *memptr = mmap(NULL,       /* let system pick where to put segment */
			      ByteSize,   /* how many bytes */
			      PROT_READ | PROT_WRITE, /* access protections */
			      MAP_SHARED, /* mapping visible to other processes */
			      fd,         /* file descriptor */
			      0);         /* offset: start at 1st byte */
	if ((struct solverShmem *)-1 == memptr)
		report_and_exit("Can't access segment...");
	
	/* create a semaphore for mutual exclusion */
	sem_t *semptr = sem_open(consSemName, /* name */
				 O_CREAT,       /* create the semaphore */
				 AccessPerms,   /* protection perms */
				 0);            /* initial value */
	if (semptr == (void *)-1)
		report_and_exit("sem_open");

	/* use semaphore as a mutex (lock) by waiting for writer to increment it */
	while (1) {
		while (!sem_wait(semptr)) { /* wait until semaphore != 0 */
			consumer(memptr);
			sem_post(semptr);
			//sleep(1);
		}
	}

	/* cleanup */
	munmap(memptr, ByteSize);
	close(fd);
	sem_close(semptr);
	unlink(BackingFile);
	return 0;
}
