#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
// Gets the current time in milliseconds

size_t	get_current_time(void)
{
	struct timeval	time;

	if (gettimeofday(&time, NULL) == -1)
		write(2, "gettimeofday() error\n", 22);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
}
int	ft_usleep(size_t ms)
{
	size_t	start;

	start = get_current_time();
	while ((get_current_time() - start) < ms)
		usleep(500);
	return (0);
}
typedef struct {
    int id;
    pthread_mutex_t *left_fork;
    pthread_mutex_t *right_fork;
    pthread_mutex_t *printmt;
    pthread_mutex_t *eating_lock;
    int time_to_eat;
    int time_to_sleep;
    int philo_num;
    int time_to_die;
    size_t start_time;
    size_t last_meal;

} philodata;

void    forkneat(philodata *data)
{
    size_t time;

    //if (data->id % 2 == 0)
    //{   
        pthread_mutex_lock(data->left_fork);
        pthread_mutex_lock(data->printmt);
                time = get_current_time() - data->start_time;

        printf("%zu, Philosopher %d picked up left fork.\n", time, data->id);
        pthread_mutex_unlock(data->printmt);
        pthread_mutex_lock(data->right_fork);
        pthread_mutex_lock(data->printmt);
                time = get_current_time() - data->start_time;

        printf("%zu, Philosopher %d picked up right fork.\n", time, data->id);
        pthread_mutex_unlock(data->printmt);
    //}
    /*else
    {
        time = get_current_time() - data->start_time;
        pthread_mutex_lock(data->right_fork);
        pthread_mutex_lock(data->printmt);
        printf("%zu, Philosopher %d picked up right fork.\n", time, data->id);
        pthread_mutex_unlock(data->printmt);
        time = get_current_time() - data->start_time;
        pthread_mutex_lock(data->left_fork);
        pthread_mutex_lock(data->printmt);
        printf("%zu, Philosopher %d picked up left fork.\n", time, data->id);
        pthread_mutex_unlock(data->printmt);
    }*/
}
void *philosopher(void *mydata) {
    philodata *data = (philodata *)mydata;
    size_t time;
    //data->start_time = get_current_time();
    while (1) {
        // Thinking
        
        pthread_mutex_lock(data->printmt);
                time = get_current_time() - data->start_time;

        printf("%zu Philosopher %d is thinking.\n",time, data->id);
        pthread_mutex_unlock(data->printmt);
        // Lock left fork
        forkneat(data);

        // Eating
         pthread_mutex_lock(data->eating_lock);
        pthread_mutex_lock(data->printmt);
            time = get_current_time() - data->start_time;
        printf("%zu, Philosopher %d is eating for %d seconds.\n", time, data->id, data->time_to_eat);
        pthread_mutex_unlock(data->printmt);
        ft_usleep(data->time_to_eat);
        data->last_meal = get_current_time();
        pthread_mutex_unlock(data->eating_lock);


        // Release forks
        pthread_mutex_unlock(data->left_fork);
        pthread_mutex_unlock(data->right_fork);
       

        pthread_mutex_lock(data->printmt);
                time = get_current_time() - data->start_time;

        printf("%zu, Philosopher %d is sleeping for %d seconds.\n", time,  data->id, data->time_to_eat);
        pthread_mutex_unlock(data->printmt);
        ft_usleep(data->time_to_sleep);
    }

    return NULL;
}

void    check_death(philodata *data)
{
    int i;

    while (1)
    {
            i = 0;

        while (i< data[0].philo_num) {
            size_t time;

            time = get_current_time();
            

            if (time - data[i].last_meal > data[i].time_to_die)
             {
                                    pthread_mutex_lock(data[i].eating_lock);

                printf("%zu Philosopher %d has died.\n", time,  data[i].id);
                                    pthread_mutex_unlock(data[i].eating_lock);

                exit(1);
            }
            i++;
        }
        ft_usleep(1);
    }

}
void    data_init(philodata *data, int philo_num)
{
    pthread_t *philosophers = (pthread_t *)malloc(philo_num * sizeof(pthread_t));
    pthread_mutex_t *forks = (pthread_mutex_t *)malloc(philo_num * sizeof(pthread_mutex_t));
    pthread_mutex_t *printmt = (pthread_mutex_t *)malloc( sizeof(pthread_mutex_t));
    pthread_mutex_t *eating_lock = (pthread_mutex_t *)malloc(philo_num * sizeof(pthread_mutex_t));
    pthread_t supervisor;
    pthread_create(&supervisor, NULL, (void *)check_death, data);
    int i = 0;

    while (i < philo_num) {
        data[i].id = i;
        data[i].left_fork = &forks[i];
        data[i].right_fork = &forks[(i + 1) % philo_num];
        data[i].time_to_eat = data[0].time_to_eat;
        data[i].start_time = get_current_time();
        data[i].time_to_sleep = data[0].time_to_sleep;
        data[i].time_to_die = data[0].time_to_die;
        data[i].philo_num = philo_num;
        data[i].printmt = printmt;
        data[i].eating_lock = &eating_lock[i];
        pthread_mutex_init(&eating_lock[i], NULL);
        pthread_mutex_init(&forks[i], NULL);
        pthread_create(&philosophers[i], NULL, philosopher, &data[i]);
        i++;
    }

      int j = 0;
    while (j < philo_num) {
        pthread_join(philosophers[j], NULL);
        pthread_mutex_destroy(&forks[j]);
        j++;
    }    pthread_join(supervisor, NULL);

        pthread_mutex_destroy(eating_lock);

    pthread_mutex_destroy(printmt);

    free(philosophers);
    free(data);
    free(forks);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <number_of_philosophers> <time_to_eat>\n", argv[0]);
        return 1;
    }
    int i;
    i = 0;
    
    int philo_num = atoi(argv[1]);
    size_t start_time = get_current_time();

    printf("%zu es start time\n", start_time);
    philodata *data = (philodata *)malloc(philo_num * sizeof(philodata));
    
    data[0].last_meal = start_time;
    data[0].time_to_eat = atoi(argv[3]);
    data[0].time_to_sleep = atoi(argv[4]);
    while(i < philo_num)
    {
            data[i].time_to_die = atoi(argv[2]);
        data[i].last_meal = data[0].last_meal;
        i++;
    }

  
   
    
    data_init(data, philo_num);

  

    return 0;
}
