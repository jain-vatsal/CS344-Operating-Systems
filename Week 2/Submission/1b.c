// including the necessary header files
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<unistd.h>

// printing the sequence for each n
void printSequence(int n){
    if(n==0)return;
    while(n!=1){
        printf("%d ",n);
        if(n&1)n=3*n+1;
        else n/=2;
    }
    if(n==1)printf("%d\n",n);
}

// main function
int main(int argc, char *argv[]){

    // importing the numbers
    char* env_var = "n";
    char* val = getenv(env_var);

    // if no number exists, we simply print for 100
    if(val==NULL){ 
        printf("no numbers are imported. Therefore print for 100\n");
        printf("for 100 : ");
        printSequence(100);
    } else {

        // printing for the different numbers that exist
        int i=0;

        // loop terminates when the input string is finished
        while(val[i]!='\0'){
            int n = 0;
            while(val[i] != ' ' && val[i]!='\0'){
                n = n*10 + val[i]-'0';
                i++;
            }
            if(val[i]==' ')i++;
            printf("for %d : ",n);
            printSequence(n);
        }
    }
    return 0;
}