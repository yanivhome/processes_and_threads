/** Compilation: gcc -o memreader memreader.c -lrt -lpthread **/
// gcc -o solver producer.c consumer.c main.c -lrt -lpthread

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include "shmem.h"

void report_and_exit(const char *msg)
{
	perror(msg);
	exit(-1);
}

void main(int argc, char **argv)
{
	int fd = shm_open(BackingFile, O_CREAT | O_RDWR, AccessPerms);  /* empty to begin */
	if (fd < 0)
		report_and_exit("Can't get file descriptor...");
	int s, i, pid;
	int num_consumers = 2, num_producers = 2;
	pthread_t solver_thread_id;
	struct Question qbuf;
	int *p, *c;
	sem_unlink(prodSemName);
	sem_open(prodSemName, /* name */
		 O_CREAT,       /* create the semaphore */
		 AccessPerms,   /* protection perms */
		 1);

	sem_unlink(consSemName);
	sem_open(consSemName, /* name */
		 O_CREAT,       /* create the semaphore */
		 AccessPerms,   /* protection perms */
		 1);

	ftruncate(fd, ByteSize); /* get the bytes */
	
	//caddr_t memptr = mmap(NULL,       /* let system pick where to put segment */
	struct solverShmem *sh = mmap(NULL,
			      ByteSize,   /* how many bytes */
			      PROT_READ | PROT_WRITE, /* access protections */
			      MAP_SHARED, /* mapping visible to other processes */
			      fd,         /* file descriptor */
			      0);         /* offset: start at 1st byte */
	if ((struct solverShmem *)-1  == sh)
		report_and_exit("Can't get segment...");
	memcpy(sh->text, "Hello World\n", sizeof("Hello World\n"));
	// = (struct solverShmem *)memptr;
	sh->prod = 0;
	sh->cons = 0;

	printf("main: prod %d, cons %d\n", sh->prod, sh->cons);

	for (i = 0; i < argc; i++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			case 'c':
				num_consumers = atoi(argv[i + 1]);
				break;
			case 'p':

				num_producers = atoi(argv[i + 1]);
				break;
			}
		}
	}
	printf("num producers %d, consumers %d\n", num_producers, num_consumers);
	for (i = 0; i < num_producers; i++) {
		pid = fork();
		if (pid == 0) {
			//	sleep(5);
			prod_main(i);
			exit(0);
		}
	}
	for (i = 0; i < num_consumers; i++) {
		pid = fork();
		if (pid == 0) {
			cons_main();
			exit(0);
		}
	}
	
	wait(NULL);
	close(fd);
	unlink(BackingFile);
}
