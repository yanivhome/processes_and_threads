#define BackingFile "/yanivBackingFile"
#define SemaphoreName "/yanivSemaphore"
#define prodSemName "/prodSem"
#define consSemName "/consSem"
#define AccessPerms 0666
// 0664
// 6 (read and write) for the owner
// 4 (readonly) for other group users
// 4 (readonly) for anyone else
#define Q_SIZE 10
enum oper {PLUS, MIN, MUL, DIV};
struct Question{
      int num1;
      int num2;
      enum oper   op;
};
struct solverShmem {
	struct Question array[Q_SIZE];
	int prod;
	int cons;
	char text[50];
};
/*
 * * Each thread sleep for n seconds
 * * Generates 2 random numbers between lowval and highval
 * * Generates random operator
 * * Stores the question in the questions array
 * * Create both threads with the same function (send n, lowval, highval to the thread function)
 * */
//struct Question array[Q_SIZE];
#define ByteSize (sizeof(struct solverShmem))
void report_and_exit(const char* msg);
int prod_main(int);
int cons_main(void);
