#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <errno.h>

typedef void (*func_type)(void);
#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

struct thread_info {
	int tnum;
        pthread_t thread_id;
};

static void *thread_start(void *arg)
{
        struct thread_info *tinfo = (struct thread_info *) arg;
	printf("I'm thread number %d (%ld)\n", tinfo->tnum, tinfo->thread_id);
	return NULL;
}
#define NUM_THREADS 5


void child(void)
{
	int id,tnum,s, cpid;
        struct thread_info tinfo[NUM_THREADS];
	void *res;

        for (tnum = 0; tnum < NUM_THREADS; tnum++) {

               /* The pthread_create() call stores the thread ID into
                  corresponding element of tinfo[] */
		tinfo[tnum].tnum=tnum;
               s = pthread_create(&tinfo[tnum].thread_id, NULL,
                                  &thread_start, &tinfo[tnum]);
               if (s != 0)
                   handle_error_en(s, "pthread_create");
           }
	for (tnum = 0; tnum < NUM_THREADS; tnum++) {
               s = pthread_join(tinfo[tnum].thread_id, &res);
               if (s != 0)
                   handle_error_en(s, "pthread_join");

    	       printf("Joined thread_id  = %d\n", tnum);
               //free(res);      /* Free memory allocated by thread */
           }

	printf("Bye\n");
		
}

void main(void) 
{
	int id, i;
	id = fork();
	if (id == 0) {
		child();
		exit(0);
	}
	wait(NULL);
	printf("father pid %d waited for child %d to complete\n", getpid(), id);
	puts ("Goodbye");
}
