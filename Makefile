CC=gcc
flag=-g -c -std=c99 -Wall -pedantic
link= -g

philo:	philo.o random.o
	$(CC) $(link) -o philo philo.o  random.o -lm

philo.o:	philo.c  random.h
	$(CC) $(flag)  philo.c -I random.h

random.o: random.c random.h
	$(CC) $(flag) random.c -I$ random.h

clean:
	rm *.o philo 
