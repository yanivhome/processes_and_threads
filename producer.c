/** Compilation: gcc -o memwriter memwriter.c -lrt -lpthread **/
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include "shmem.h"

struct prod_info {
	int *p;
	int send_n;
	int lowval;
	int highval;
	struct solverShmem *memptr;
};

static int producer(void *arg)
{
	struct prod_info *info = (struct prod_info *)arg;
	struct solverShmem *shmem = info->memptr;
	
	char kk[100];
	int q_prod = shmem->prod;
	int q_cons = shmem->cons;
	
	if (((q_prod + 1) % Q_SIZE) == q_cons ) {
		return;
	}
	printf("producer: Producer %d Consumer %d\n", q_prod, q_cons);
	shmem->array[q_prod].num2 = rand() % (info->lowval);
	shmem->array[q_prod].num1 = rand() % (info->highval);
	shmem->array[q_prod].op   = rand() % 4;
	sprintf(kk, "Produce %d %d %d sleep %d", shmem->array[q_prod].num1, shmem->array[q_prod].num2, shmem->array[q_prod].op, info->send_n);
	q_prod = (q_prod + 1) % Q_SIZE;
	shmem->prod = q_prod;
	puts(kk);
	return info->send_n;
}

int prod_main(int prod_id)
{
	struct prod_info info;
	struct solverShmem *sh;
	info.send_n = rand() % 5;
	info.lowval = rand() % 10;
	info.highval = rand() % 20;
	int val;
	int fd = shm_open(BackingFile,      /* name from smem.h */
			  O_RDWR, /* read/write, create if needed */
			  AccessPerms);     /* access permissions (0644) */
	if (fd < 0)
		report_and_exit("Can't open shared mem segment...");

	sh = mmap(NULL,       /* let system pick where to put segment */
			      ByteSize,   /* how many bytes */
			      PROT_READ | PROT_WRITE, /* access protections */
			      MAP_SHARED, /* mapping visible to other processes */
			      fd,         /* file descriptor */
			      0);         /* offset: start at 1st byte */
	if ((struct solverShmem *)-1  == sh)
		report_and_exit("Can't get segment...");

	fprintf(stderr, "shared mem address: %p [0..%d]\n", sh, ByteSize - 1);
	fprintf(stderr, "backing file:       /dev/shm%s\n", BackingFile);
	info.memptr = sh;
	printf("prod text %s\n", sh->text);
	/* semaphore code to lock the shared mem */
	sem_t *semptr = sem_open(prodSemName, /* name */
				 O_CREAT,       /* create the semaphore */
				 AccessPerms,   /* protection perms */
				 0);            /* initial value */
	sem_getvalue(semptr, &val);
	//printf("val %d prod_id %d\n", val, prod_id);
	if (semptr == (void *)-1)
		report_and_exit("sem_open");
	
	while (1) {
		while (!sem_wait(semptr)) { /* wait until semaphore != 0 */
			
			val = producer(&info);
			sem_post(semptr);
			sem_getvalue(semptr, &val);
			//sleep(val);
		}
	}

	/* clean up */
	munmap(sh, ByteSize); /* unmap the storage */
	close(fd);
	sem_close(semptr);
	shm_unlink(BackingFile); /* unlink from the backing file */
	return 0;
}
