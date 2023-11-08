#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>

typedef struct s_philo {
    int id;
    pthread_mutex_t *r_fork;
    pthread_mutex_t *l_fork;
} t_philo;

size_t get_current_time(void) {
    struct timeval time;

    if (gettimeofday(&time, NULL) == -1) {
        write(2, "gettimeofday() error\n", 22);
    }
    return (time.tv_sec * 1000 + time.tv_usec / 1000);
}
char myfork[4];

// Carry out a deposit
void *deposit(void *amount) {
    int i;
    char forks = 'o';
    i = 0;
    int id = ((t_philo *)amount)->id;

    while (i < 10) {
        if(myfork[id +1] != 'o')
        pthread_mutex_lock(((t_philo *)amount)->r_fork);
        myfork[id] = 'o';
        if (forks = 'l')
          forks = 'b';
        else
          forks = 'r';
        printf("Philosopher %i got the right fork\n", id);
          pthread_mutex_lock(((t_philo *)amount)->l_fork);
          if (forks == 'r')
            forks = 'b';
          else
            forks = 'r';
        printf("Philosopher %i got the left fork\n", id);
        if (forks == 'b')
        {
          printf("Philosopher %i is eating\n", id);
          //printf("Time is %zu\n", get_current_time());
          usleep(5000);
          printf("Philosopher %i finished eating\n", id);
          pthread_mutex_unlock(((t_philo *)amount)->r_fork);
          pthread_mutex_unlock(((t_philo *)amount)->l_fork);
          forks = 'o';
          myfork[id] = 'f';
        }
         //printf("Time is %zu\n", get_current_time());
        //printf("Philosopher %i is sleeping\n", id);
        //printf("Time is %zu\n", get_current_time());
        usleep(10000);
        //printf("Philosopher %i finished sleeping\n", id);
        //printf("Time is %zu\n", get_current_time());

        i++;
    }

    return NULL;
}

int main() {
    // Mutex variable
    t_philo philo1;
    t_philo philo2;
    t_philo philo3;


    // Output the balance before the deposits

    // We'll create two threads to conduct a deposit using the deposit function
    pthread_t philos1;
    pthread_t philos2;
    pthread_t philos3;

    // Allocate memory for the mutexes
    philo1.r_fork = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    philo2.r_fork = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    philo3.r_fork = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    philo1.l_fork = philo3.r_fork;
    philo2.l_fork = philo1.r_fork;
    philo3.l_fork = philo2.r_fork;
    // Initialize the mutex
    pthread_mutex_init(philo1.r_fork, NULL);
    pthread_mutex_init(philo2.r_fork, NULL);
    pthread_mutex_init(philo3.r_fork, NULL);

    philo1.id = 1;
    philo2.id = 2;
    philo3.id = 3;

    // Create threads
    pthread_create(&philos1, NULL, deposit, (void *)&philo1);
    pthread_create(&philos2, NULL, deposit, (void *)&philo2);
    pthread_create(&philos3, NULL, deposit, (void *)&philo3);

    // Join the threads
    pthread_join(philos1, NULL);
    pthread_join(philos2, NULL);
    pthread_join(philos3, NULL);

    // Destroy the mutex
    pthread_mutex_destroy(philo1.r_fork);
    pthread_mutex_destroy(philo2.r_fork);
    pthread_mutex_destroy(philo3.r_fork);

    // Free the allocated memory for the mutexes
    free(philo1.r_fork);
    free(philo2.r_fork);
    free(philo3.r_fork);

    return 0;
}