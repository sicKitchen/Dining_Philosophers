//
//  philo.c
//  philo
//
//  Created by Spencer Kitchen on 3/20/16.
//
//

#define _SVID_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <assert.h>
#include <unistd.h>
#include "random.h"
#include <stdlib.h>
#include <sys/wait.h>

#define PHILO_MAX 5         // number of philosiphers
#define MAX_EAT_TIME 100    // how long philosophers will eat for
//---------------------------------------------------------------------
int eat_time();             // gets random eat time
int think_time();           // gets random think time
void free_semaphore();
//---------------------------------------------------------------------

// UNCOMMENT IF SEMUN IS NOT DEFINED IN SYS/SEM.h
// im being lazy here and not checking myself, up to user
/*
union semun {
    int              val;    // Value for SETVAL 
    struct semid_ds *buf;    // Buffer for IPC_STAT, IPC_SET 
    unsigned short  *array;  // Array for GETALL, SETALL 
    struct seminfo  *__buf;  // Buffer for IPC_INFO
                             // (Linux-specific) 
};
*/
int chopSticks;             // hold the 5 chopstick semephores
pid_t philosopher[5];       // holds processes, 1 for each philosopher
int philo_num = 0;

//=====================================================================
int main(int argc, const char * argv[]) {
    int still_eating = PHILO_MAX;   // wait for each philosopher
    
    // make semephore for each chopstick and initilize
    assert(chopSticks = semget(IPC_PRIVATE, PHILO_MAX,
                               IPC_CREAT | S_IRUSR | S_IWUSR));
    union semun arg;        // create 4th argument for semctl
    arg.val = 1;            // value of argument set to 1
    
    // set all chopsticks to have value of 1
    for (int i = 0; i < PHILO_MAX; i++) {
        if (semctl(chopSticks, i, SETVAL, arg) < 0) {
            fprintf(stderr, "initilization of semephore failed\n");
            free_semaphore();
            exit(-1);
        }
    }
    
    // spawn a proccess for every philospher
    for (int i = 0; i < PHILO_MAX; i++) {
        // PARENT
        if ((philosopher[i] = fork())) {
            // philo_count gets cloned every time new process spawns
            // with current count number
            philo_num++;
        }
        // CHILD = philosophers
        else {
            int time_eat = 0;      // holds time spent eating
            int time_think = 0;    // holds time spent thinking
           
            /*------------------------------------------------------\
            | STRUCTURES for unlock/lock chopsticks                 |
            | RIGHT chopstick equals philosopher number             |
            | LEFT chopstick equals (philosopher number +1) mod 5   |
            |                                                       |
            | philo num :         0   1   2   3   4                 |
            |                    / \ / \ / \ / \ / \                |
            | chop num  : Right 0   1   2   3   4   0 Left          |
            \------------------------------------------------------*/
            int _philo_num = (philo_num +1)%5;
            // LOCK the chopstick to the RIGHT of philosopher
            struct sembuf lockRight[1] = {{philo_num, -1, 0}};
            // UNLOCK chopstick to RIGHT of philosopher
            struct sembuf unlockRight[1] = {{philo_num, 1, 0}};
            // LOCK chopstick to LEFT of philosopher
            struct sembuf lockLeft[1] = {{_philo_num, -1, IPC_NOWAIT}};
            // UNLOCK chopstick to LEFT of philosopher
            struct sembuf unlockLeft[1] = {{_philo_num, 1, 0}};
            
            while (time_eat < MAX_EAT_TIME) {
                int time;              // temp time variable
                // TRY TO EAT
                // pick up RIGHT chopstick
                if (semop(chopSticks, lockRight, 1) == 0) {
                    
                    // TRY to pick up LEFT chopstick
                    if (semop(chopSticks, lockLeft, 1) == 0) {
                        
                        // HAVE BOTH CHOPSTICKS HERE
                        time = eat_time();
                        printf("[philosopher %d] EATING for:\t%3d seconds    Total time eating:\t%3d seconds\n", philo_num, time, time_eat);
                        time_eat = time_eat + time;
                        sleep(time);
                        // put down both chopsticks
                        semop(chopSticks, unlockLeft, 1);
                        semop(chopSticks, unlockRight, 1);
                    }
                    // CANT pick up LEFT chopstick
                    else {
                        //drop RIGHT chopstick
                        semop(chopSticks, unlockRight, 1);
                    }
                }
                
                // THINK after eating
                time = think_time();
                printf("[philosopher %d] THINKING for:\t%3d seconds\n", philo_num, time);
                time_think = time_think + time;
                sleep(time);
            }
            // DONE eating
            printf("\n***DONE*** [philosopher %d] Eating total = %d seconds    Thinking total = %d seconds\n\n", philo_num, time_eat, time_think);
            exit(0);
        }
    }
    
    // WAIT for all 5 philosophers to finish
    int status = 0;
    pid_t wait_pid;             // pid of child waiting for
    while (still_eating > 0) {
        wait_pid = wait(&status);
        printf("<Child with PID %ld exited with status %d>\n", (long)wait_pid, status);
        --still_eating;
    }
    free_semaphore();
    return 0;
}
//======================================================================
// returns random time based on
// mean time = 9 & standard dev = 3
int eat_time() {
    int time = randomGaussian(9, 3);
    if (time < 0) time = 0;
    return time;
}
//----------------------------------------------------------------------
// returns random time based on
// mean time = 11 & standard dev = 7
int think_time() {
    int time = randomGaussian(11, 7);
    if (time < 0) time = 0;
    return time;
}
//----------------------------------------------------------------------
// free semaphores
void free_semaphore() {
    if (semctl(chopSticks, PHILO_MAX, IPC_RMID) < 0) {
        fprintf(stderr, "Could not free semaphores\n");
        exit(-1);
    }
}
