#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
typedef void (*func_type)(void);

void mytask(int i)
{
   char str[10];
   sprintf(str, "task %d", i);
   while(1){
		puts(str);
                sleep(5);
                }

}
struct process {
	int pid;
}; 
void child(void)
{
	int id,i,s, cpid;
	
	struct process pr_arr[5];
	for (i=0;i<5;i++)
	{
		pr_arr[i].pid = fork(); 
		if(pr_arr[i].pid == 0)
		{
			mytask(i);
			exit(0);
		}
		
	}
	
        printf("Parent pid = %d\n", getpid());
	while (1) {
		cpid = wait(NULL); /* reaping parent */
		
    		printf("Child pid = %d is dead -> Resurecting\n", cpid);
		for (i = 0; i < 5; i++) {
			if (pr_arr[i].pid == cpid) {
				pr_arr[i].pid = fork();
		                if(pr_arr[i].pid == 0) {
		                        mytask(i);
                		        exit(0);
		                }
				break;
			}
		}
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
