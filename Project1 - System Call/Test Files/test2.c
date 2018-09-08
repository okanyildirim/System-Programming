#include <stdlib.h>

int main(void){

pid_t pid=0, pid1=0;
getchar();
pid=fork();  //Create the process
getchar();
pid1=fork(); //Create the process
getchar();

exit(EXIT_SUCCESS); // kill process

// by renice these process nice values, we test our exit system call
}
