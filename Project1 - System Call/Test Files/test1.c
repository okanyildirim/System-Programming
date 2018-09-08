#include <stdio.h> // required libraries
#include <errno.h> // for errno value
#include <string.h> // for strerrno function

#define NR_set_myFlag 356

int main(int argc, char *argv[]){ // taking argument from terminal
	
int pid=atoi(argv[1]); // pid argument 
int flag=atoi(argv[2]);// flag argument
errno=0 ; // declaration for errno
long result;
result=syscall(NR_set_myFlag,pid,flag); // return value of system call
int errnumber=errno;

if(result==0){
    printf("%s\n", strerror(errno)); // if result is 0, system call is successfull
}

else{ // return -1, there is errno
    if (errnumber == EPERM) {
        printf("Error:%s\n ", strerror(errno)); //strerror is a meaningful string error
    }
    else if (errnumber == EINVAL) {
        printf("Error:%s\n ", strerror(errno));
    }
   else if (errnumber == ESRCH) {
        printf("Error:%s\n ", strerror(errno));
    }
    else 
    printf("Another error\n");
}
return 0;
}
