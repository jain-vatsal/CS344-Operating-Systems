// including the necessary header files
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<unistd.h>

// printing the sequence for each n
void printSequence(int n){
    while(n!=1){
        printf("%d ",n);
        if(n&1)n=3*n+1;
        else n/=2;
    }
    if(n==1)printf("%d\n",n);
}


int main(int argc, char *argv[]){
    int n;
    // if no input is mentioned explicitly
    if(argv[1] == NULL)printSequence(100);

    // for different numbers mentioned in the command line
    else for(int i=1;i<argc;i++)printSequence(atoi(argv[i])); 

    return 0;
}