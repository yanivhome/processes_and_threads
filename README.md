# Various threads and processes thecniques
## This depo contains multiple programs:
## task1.c 
Reference: https://devarea.com/lab-processes-and-threads
compile: gcc -c task1 task1.c 
When the program start fork a new child process. The father process should wait for the child process to finish.
The child process main thread should spawn 5 additional threads.
Each thread should print: “I am thread number n” and terminate.
The child process main thread should wait for all it’s threads to terminate. It should then print “Bye” and exit.
When the child exists, the father should print “Goodbye” and exit

## task2.c
Reference: https://devarea.com/lab-processes-and-threads
compile: gcc -c task2 task2.c
build 5 processes that runs forever – each one print message every 5 seconds 
the main process create its children and wait until they die
if a child process die – the main create it again

## mathq.c
compile: gcc -c mathq mathq.c
Reference: https://devarea.com/lab-threads-and-synchronization/
This program creates 2 producers (threads):
- Each thread sleep for n seconds
- Generates 2 random numbers between lowval and highval
- Generates random operator
- Stores the question in the questions array
-  Create both threads with the same function (send n, lowval, highval to the thread function)

Create a solver thread
- Sleeps for 5 seconds
- Check if a new question exists
- Solve it and print result
- Use semaphorsand mutexes to access the shared data structures

## solver: main.c consumer.c producer.c
compile: gcc -o solver consumer.c main.c producer.c -lpthread -lrt
Change the above to use processes instead of threads

Add command line arguments to declare the number of consumers and producers

for example to create 2 producers and 3 consumers run: ./solver -p 2 -c 3
