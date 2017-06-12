# Dining Philosophers

**Problem statement:**

Five silent philosophers sit at a round table with bowls of rice. Chopsticks are placed between each pair of adjacent philosophers.

Each philosopher must alternately think and eat. However, a philosopher can only eat rice when they have both left and right chopsticks. Each chopstick can be held by only one philosopher and so a philosopher can use the chopstick only if it is not being used by another philosopher. After an individual philosopher finishes eating, they need to put down both chopsticks so that the chopsticks become available to others. A philosopher can take the chopstick on their right or the one on their left as they become available, but cannot start eating before getting both chopsticks.

Eating is not limited by the remaining amounts of rice or stomach space; an infinite supply and an infinite demand are assumed.

The problem is how to design a discipline of behavior (a concurrent algorithm) such that no philosopher will starve; i.e., each can forever continue to alternate between eating and thinking, assuming that no philosopher can know when others may want to eat or think.

[wiki](https://en.wikipedia.org/wiki/Dining_philosophers_problem)

## Description

"philo" (source file named "philo.c").

This program forks 5 child processes, each representing one philosopher. Using semaphores, each philosopher repeats a cycle of eating and thinking until all philosophers have eaten for at least 100 seconds (at which point a philosopher will "leave the table"... terminate).

There are 5 chopsticks at the table (one between each pair of adjacent philosophers). In order to eat, a philosopher must acquire both the chopsticks adjacent to him/her. If he/she cannot, they are made to wait until they can. The wait time does not count as thinking time. Once they have a pair of chopsticks, they eat for the expected period of time and then replace (release) the chopsticks on the table and begin their period of thinking. Each philosopher repeats this process until they have eaten for a total of 100 seconds or more.

"randomGaussian" is a routine to simulate the random amounts of time that a philosopher eats and thinks. The mean eating and thinking times shall be 9 and 11 seconds, respectively, with standard deviations of 3 and 7 seconds, respectively. Obviously, if randomGaussian returns a negative time value, treat it as zero. Simulation of the eating or thinking time is done by calling sleep() for the requisite number of seconds.

Chopsticks are represented an array of IPC semaphores, each representing one chopstick.

## Archive

```txt
Dining_Philosophers
  ├── Makefile              : Builds program
  ├── README.md             : Readme for program
  ├── philo.c               : Source code
  ├── random.c              : Returns random number from Gaussian distribution
  └── random.h              : header for random.c
```

## Building and Running

* Some systems have `union semun` defined in `SYS/SEM.h` (OSX), If it is defined comment out this block of code.
* If `union semun` is not defined in `SYS/SEM.h` then leave in.
* (philo.c, line: 32-38)

```c
union semun {
    int              val;    // Value for SETVAL 
    struct semid_ds *buf;    // Buffer for IPC_STAT, IPC_SET 
    unsigned short  *array;  // Array for GETALL, SETALL 
    struct seminfo  *__buf;  // Buffer for IPC_INFO
                             // (Linux-specific) 
};
```

* Open file location in terminal, enter `make` to build.

```c
make
```

* To delete enter:

```c
make clean
```

* To run program enter:

```c
./philo
```

* Program will terminate itself when completed or enter `ctrl-c` to force quit

## Author

* **Spencer Kitchen**
* Email: <sckitchen.dev@gmail.com>