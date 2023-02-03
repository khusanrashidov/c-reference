// sample foo
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char* argv[]){
    
    int i = 0;
    
    int s = 0;
    int n = atoi(argv[1]);
    
    for(i=0; i<n; i++){
        s += i;
        sleep(1);
    }
    
    printf("In child process: Process %d exit.\n", getpid());
    
    return 0;
}
