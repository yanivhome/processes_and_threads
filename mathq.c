#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

enum oper {PLUS, MIN, MUL, DIV};
pthread_mutex_t lock;

struct Question{
      int num1;
      int num2;
      enum oper   op;
};
/*
* Each thread sleep for n seconds
* Generates 2 random numbers between lowval and highval
* Generates random operator
* Stores the question in the questions array
* Create both threads with the same function (send n, lowval, highval to the thread function)
*/
#define Q_SIZE 10
int q_prod = 0;
int q_cons = 0;

struct Question array[Q_SIZE];
struct prod_info {
	pthread_t thread_id;
	int send_n;
	int lowval;
	int highval;
};
static void* producer(void *arg) {
	struct prod_info *info = (struct prod_info *)arg;
	printf("Producer %d %d %d\n", info->lowval, info->highval, info->send_n);
	char kk[100];
	while (1) {
		pthread_mutex_lock(&lock);
		if (q_prod == (q_cons + 1) % Q_SIZE) {
	 		pthread_mutex_unlock(&lock);
			continue;
		}
		array[q_prod].num2 = rand()%(info->lowval);
		array[q_prod].num1 = rand()%(info->highval);
		array[q_prod].op   = rand()%4;
		sprintf(kk, "Produce %d %d %d sleep %d", array[q_prod].num1, array[q_prod].num2, array[q_prod].op,info->send_n);
		q_prod = (q_prod + 1)%Q_SIZE;
		pthread_mutex_unlock(&lock);
		puts (kk);
		sleep(info->send_n);
	}
}
/*
Create a solver thread
Sleeps for 5 seconds
Check if a new question exists
Solve it and print result
*/
static void* solver(void* arg) 
{
	char str[100];
	puts("Consumer");
	while (1) {
		sleep(5);
		if (q_prod != q_cons) {
			struct Question *q = &array[q_cons];
			switch (q->op) {
			case PLUS:
				sprintf(str, "Result of %d + %d = %d\n", q->num1, q->num2, q->num1+q->num2);
				break;
			case MIN:
				sprintf(str, "Result of %d - %d = %d\n", q->num1, q->num2, q->num1-q->num2);
				break;
			case MUL:
				sprintf(str, "Result of %d * %d = %d\n", q->num1, q->num2, q->num1*q->num2);
				break;
			case DIV:
				if (q->num2 == 0)
					sprintf(str, "Cannot divide by zero\n");
				else
					sprintf(str, "Result of %d / %d = %d\n", q->num1, q->num2, q->num1/q->num2);
				break;
			default:
				sprintf(str, "Error: unknown op %d\n", q->op);
				break;
			}
			q_cons = (q_cons + 1) % Q_SIZE;
			puts (str);
                }
	}
}

void main(void) 
{
	int s, i;
	pthread_t solver_thread_id;

	struct prod_info info[2] = {{
                .send_n = 3,
                .lowval = 5,
                .highval = 10,
        },
	{
                .send_n = 4,
                .lowval = 10,
                .highval = 90,
        }};
	if (pthread_mutex_init(&lock, NULL) != 0) {
        	printf("\n mutex init has failed\n");
	        return ;
	}
	for (i = 0; i < 2; i++) {
		s = pthread_create(&info[i].thread_id, NULL,
                           &producer, &info[i]);
	        if (s != 0) {
        	         puts("pthread_create producer");
			return;
		}
	}
	s = pthread_create(&solver_thread_id, NULL,
                           &solver, &info[0]);
	if (s != 0) {
               puts("pthread_create solver");
		return;
	}
	while (1) {}
	
}
