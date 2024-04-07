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

// Global varis
void *shmaddr;
pthread_mutex_t mutex;
int sum_of_estimates = 0;
int total_counter=0;

struct Robot {
    int id;
    int x_coordinate;
    int y_coordinate;
    int estimate;
    double average_estimate;
};

int main(int argc, char *argv[])
{
    int i = strtol(argv[1], NULL, 10);
    int exit_width = strtol(argv[2], NULL, 10);
    int frame_width = strtol(argv[3], NULL, 10);
   
    srand(time(0));
    int shmid, j;
    
    // unique key
    key_t key = ftok("s", 's');

    // Create shared memory
    shmid = shmget(key, SHM_SIZE, 0666);

    // Attach shared memory 
    shmaddr = shmat(shmid, NULL, 0);

    // Robot array
    struct Robot *robots = (struct Robot *)shmaddr;
    
    // Child process
    srand((unsigned int)(time(NULL) ^ getpid()));

    // robot structure for the child process
    struct Robot robot;
    // robo ids
    robot.id = i + 1;
    // robo cordinates
    robot.x_coordinate = rand() % 101; 
    robot.y_coordinate = rand() % 101; 
    
    // robot's frame 
    int robot_frame = (robot.x_coordinate / frame_width);

    // estimate based on frame distance from the exit width
    if (rand()%2==1)
    {
    	robot.estimate = exit_width - robot_frame;
    }
    else
    {
    	robot.estimate = exit_width + robot_frame;
    }
    
    printf("Child process %d with PID: %d\n", robot.id, getpid());
    printf("X-coordinate: %d, Y-coordinate: %d, Estimate: %d , robot_frame: %d\n", robot.x_coordinate, robot.y_coordinate, robot.estimate, robot_frame);

    robots[i] = robot;
    
    sleep(4);
    
    // Calculating distances + save estimates for robots that are in less than 5 units radius
    int count =0;
    for (j = 0; j < MAX_ROBOTS; ++j) 
    {
        if (i != j) 
        {
            // eucladian distance between robots
            double distance = sqrt(pow(robot.x_coordinate - robots[j].x_coordinate, 2) + pow(robot.y_coordinate - robots[j].y_coordinate, 2));
           
            // distance threshold check
            if (distance <= DISTANCE_THRESHOLD) 
            {
            	count++;
                // estimates values of 5 unit radius robos
                robot.average_estimate += robots[j].estimate;
            }
        }
    }
    // avg estimate
    robot.average_estimate /= count; 
    if (count == 0)
    {
    	// avg estimate og itself if no friends 
    	robot.average_estimate = robot.estimate;
    }
    
    robots[i] = robot;
    
    printf("Robot %d calculated average estimate: %.2f\n", robot.id, robot.average_estimate);
   
    shmdt(shmaddr);
    exit(0);
}
