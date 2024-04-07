#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <math.h>
#include <sys/wait.h>
#include <errno.h>
#include <pthread.h>

int MAX_ROBOTS = 50;
float DISTANCE_THRESHOLD = 5.0;
int FRAMES = 10;

#define SHM_SIZE sizeof(struct Robot) * MAX_ROBOTS

// Global vars
void *shmaddr;
pthread_mutex_t mutex;
int sum_of_estimates = 0;
int total_counter=0;

struct Robot 
{
	int id;
	int x_cgoordinate;
	int y_coordinate;
	int estimate;
	double average_estimate;
};


// Calculate sum of estimate for a robo
void *calculate_sum(void *arg) 
{
	struct Robot *robot = (struct Robot *)arg;
	pthread_mutex_lock(&mutex); 
	if (robot->average_estimate != 0.00)
	{
		total_counter++;
		sum_of_estimates = sum_of_estimates + int(robot->average_estimate);
	}
	pthread_mutex_unlock(&mutex); 
	usleep(500000);
	pthread_exit(NULL);
}

int main() 
{
	srand(time(0));
	int shmid, i, j;

	// unique key
	key_t key = ftok("s", 's');

	// Create shared memory 
	shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);

	// Attach shared memory 
	shmaddr = shmat(shmid, NULL, 0);
	
	// exit point coordinates
	int exit_x=0;
	int exit_y=0;
	
	// exit width
	int exit_width = 16 + rand() % 8; 
	printf("Exit_width: %d\n", exit_width);

	int check_axis= rand() % 4+1;
	printf("check_axis %d\n", check_axis);
	// if 1 => x=0 , generate y [left wala]
	// if 2 => x =100, generate y [right wala]
	// if 3 => y=0 , generate x [bottom] 
	// if 4 => y= 100, generate x [up]
    
	if(check_axis=1)
	{
		exit_x = 0; 
		exit_y = rand() % 101;
	}

	if(check_axis=2)
	{
		exit_x = 100; 
		exit_y = rand() % 101;
	}

	if(check_axis=3)
	{
		exit_x = rand() % 101; 
		exit_y =0;
	}

	if(check_axis=4)
	{
		exit_x = rand() % 101; 
		exit_y =100;
	}

	printf("\n");
	printf("Exit Point: X = %d, Y = %d, Width = %d\n", exit_x, exit_y, exit_width);
	printf("\n");

	// frame width
	int frame_width = 100 / FRAMES;

	// robot array
	Robot *robots = (Robot *)shmaddr;

	for (i = 0; i < MAX_ROBOTS; ++i) 
	{
	 int pid = fork();

		if (pid < 0) 
		{
		    printf("Forking Error\n");
		    return 1;
		} 
		else if (pid == 0) 
		{
		 	// Child process in exec
			
		 	char i_2_char[20]; 
		 	snprintf(i_2_char, 20, "%d", i);
		 	
		 	char exit_width_to_char[20]; 
		 	snprintf(exit_width_to_char, 20, "%d", exit_width);
		 	
		 	char frame_width_to_char[20]; 
		 	snprintf(frame_width_to_char, 20, "%d", frame_width);
		 	
		 	char *args[]={"./shmadder", i_2_char, exit_width_to_char,frame_width_to_char ,NULL}; 
		       
		 	execvp("./shmadder",args );

		} 
		else 
		{
		    // Parent process - continue to fork
		}
	}


	// Parent process waits for children to complete
	if (i == MAX_ROBOTS) 
	{
		for (i = 0; i < MAX_ROBOTS; ++i) 
		{
		    wait(NULL);
		}
		
		printf("\n");
		printf("All child processes completed.\n");
		printf("\n");
			
		// Initialize mutex
		pthread_mutex_init(&mutex, NULL);


		// Array for thread IDs
		pthread_t tid[MAX_ROBOTS];

		// threads for each robot -> calculate sum
		for (i = 0; i < MAX_ROBOTS; ++i) 
		{
			pthread_create(&tid[i], NULL, calculate_sum, (void *)&robots[i]);
		}

		//printf("Total_counter: %d\n", total_counter);
		int avg_width=sum_of_estimates/total_counter;
		printf("Average Width %d\n", avg_width);

		for (i = 0; i < MAX_ROBOTS; ++i) 
		{
			pthread_join(tid[i], NULL);
		}


		printf("Sum of all Estimates: %d\n", sum_of_estimates);
		int x1 = 0;

		for (i = 0; i < MAX_ROBOTS; ++i) 
		{
			x1 += robots[i].average_estimate;
		}

		printf("Total estimate actual= %d\n", x1);

		// Detach from and remove the shared memory segment
		shmdt(shmaddr);
		shmctl(shmid, IPC_RMID, NULL);

		// Destroy mutex
		pthread_mutex_destroy(&mutex);
g
	}
    return 0;
}
