#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h> 
#include <signal.h>


int random_inc(int cur)
{
	if(100 - cur > 10) return rand() % 10 + 1;
	else return 100 - cur; 	
}

int optimal_inc(int choice)
{
	if(100 - choice > 10) return 11 - choice;
	else return 100 - choice; 	
}

int main(int argc, char* argv[]){
    srand(time(0));
    int p1[2];  //Child -> Parent 
    int p2[2];  //Parent -> Child   

    
    if(pipe(p1) == -1){
        return 1;
    }
    if(pipe(p2) == -1){
        return 1;
    }
    
    int pid = fork();
    if(pid == -1){
        return 2;
    }

    if(pid == 0){
        //Child process
        close(p1[0]);       //No need for first pipe to read
        close(p2[1]);       //No need for second pipe to write
        
        int sum;
        int choice;
        int arr[2];

        while(read(p2[0], &arr, sizeof(int) * 2) < 100){
            //read array from parent and assign to variables
            sum = arr[0];
            choice = arr[1];
            
            //assign new values to variables
            choice = random_inc(sum);
            sum += choice;
            printf("Player 2: %d\n", sum);

            arr[0] = sum;
            arr[1] = choice;

            if(sum == 100){
                kill(0, SIGTERM);
            }
            write(p1[1], &arr, sizeof(int) * 2);
        }
        close(p2[0]);
        close(p1[1]);
    }
    else{
        //Parent Process uses optimal winning method
        //Player 1 starts and chooses 1 at first
        int choice = 1;
        int sum = 1;
        int arr[2];

        //assign original values to array thats being sent
        arr[0] = sum;
        arr[1] = choice;

        close(p2[0]);   //no need to read from parent   
        close(p1[1]);   //no need for child to write to parent
        
        while(sum < 100){
            write(p2[1], &arr, sizeof(int) * 2);
            read(p1[0], &arr, sizeof(int) * 2);

            //assign variables to newly read values
            sum = arr[0];
            choice = arr[1];

            printf("Array in parent: ");
            for (int i = 0; i <= 1; i++) {
                printf("%d ", arr[i]);
            }
            printf("\n");
            
            choice = optimal_inc(arr[1]);
            sum += choice;
            printf("Player1: %d\n", sum);

            arr[0] = sum;
            arr[1] = choice;

            if(sum == 100){
                kill(0, SIGTERM);
            }
        }
        close(p1[0]);
        close(p2[1]);
    }
    waitpid(pid, 0, 0);
    return 0;
}