#include "philotest.h"

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

int    dead_philo(s_philo *philo)
{
    pthread_mutex_lock(philo->s_philodata->death_lock);
    if (philo->s_philodata->philosopher_died == 10) 
    {
        pthread_mutex_unlock(philo->s_philodata->death_lock);
        return 1;
    }
    pthread_mutex_unlock(philo->s_philodata->death_lock);
    return(0);
}

int    thinknsleep(s_philo *philo)
{
    size_t  time;
    int     tt_die;
    int     tt_sleep;

    tt_sleep = philo->s_philodata->time_to_sleep;
    tt_die = philo->s_philodata->time_to_die;
    if (dead_philo(philo) == 1)
            return(1);
    time = get_current_time() - philo->s_philodata->start_time;
    printf("%zu %d is sleeping\n", time, philo->id);
    ft_usleep(philo->s_philodata->time_to_sleep);
    time = get_current_time() - philo->s_philodata->start_time;
    if (dead_philo(philo) == 1)
            return(1);
    printf("%zu %d is thinking\n", time, philo->id);
    if (philo->s_philodata->philo_num % 2 != 0)
        ft_usleep((philo->s_philodata->time_to_eat * 2) - tt_sleep);
    else
        ft_usleep(((tt_die- (philo->s_philodata->time_to_eat + tt_sleep))/2)-2);
    return(0);

}

int    even_fork(s_philo *philo)
{
    size_t  time;

    pthread_mutex_lock(philo->left_fork);
    if (dead_philo(philo) == 1)
    {
        pthread_mutex_unlock(philo->left_fork);
         return(1);
    }
    time = get_current_time() - philo->s_philodata->start_time;
    printf("%zu %d has taken a fork\n", time, philo->id);
    pthread_mutex_lock(philo->right_fork);
    if (dead_philo(philo) == 1)
    {
        pthread_mutex_unlock(philo->right_fork);
        pthread_mutex_unlock(philo->left_fork);
         return(1);
    }
    time = get_current_time() - philo->s_philodata->start_time;
    printf("%zu %d has taken a fork\n", time, philo->id);
    return(0);
}

int    odd_fork(s_philo *philo)
{
    size_t  time;

    pthread_mutex_lock(philo->right_fork);
    if (dead_philo(philo) == 1)
    {
        pthread_mutex_unlock(philo->right_fork);
         return(1);
    }
    time = get_current_time() - philo->s_philodata->start_time;
    printf("%zu %d has taken a fork\n", time, philo->id);
    pthread_mutex_lock(philo->left_fork);
    if (dead_philo(philo) == 1)
    {
        pthread_mutex_unlock(philo->right_fork);
        pthread_mutex_unlock(philo->left_fork);
         return(1);
    }
    time = get_current_time() - philo->s_philodata->start_time;
    printf("%zu %d has taken a fork\n", time, philo->id);
    return(0);
}

int    pick_fork(s_philo *philo)
{
    if(get_current_time()- philo->s_philodata->start_time < 30)
	{
		if (philo->id % 2 != 0)
        {   
            if(odd_fork(philo) == 1)
                return(1);
        }
		else
        {
            if(even_fork(philo) == 1)
                return(1);
        }
    }
	else
        if(even_fork(philo) == 1)
            return(1); 
    return(0);
}

void *p_routine(void *arg) {
    s_philo *philo = (s_philo *)arg;
    size_t  time;

    int i;
    i = 0;
    while (i < 10) 
    {
        if(pick_fork(philo) == 1)
         //   return(NULL);
        //pthread_mutex_lock(philo->eating_lock);
        time = get_current_time() - philo->s_philodata->start_time;
        printf("%zu %d is eating\n", time, philo->id);
        philo->last_meal = get_current_time();
        ft_usleep(philo->s_philodata->time_to_eat);
        //pthread_mutex_unlock(philo->eating_lock);
        pthread_mutex_unlock(philo->right_fork);
        pthread_mutex_unlock(philo->left_fork);
        if(thinknsleep(philo) == 1)
            return(NULL);
        if (dead_philo(philo) == 1)
            return(NULL);
        i++;
    }
    return NULL;
}

void    *superv(void *arg)
{
    int     i;

    s_philodata *data = (s_philodata *)arg;
    struct t_philo *p_list = data->philo_list;
    size_t current_time;

    while (1)
	{
		i = 0;
        //printf("hola\n");
		while (i < data->philo_num) 
		{
            pthread_mutex_lock(data->philo_list[i].eating_lock);
            if (get_current_time() - data->philo_list[i].last_meal > (size_t)data->time_to_die)
            {
                pthread_mutex_unlock(p_list[i].eating_lock);
                pthread_mutex_lock(data->death_lock);
                current_time = get_current_time() - data->start_time;
                data->philosopher_died = 10;
                printf("%zu %d died\n", current_time,  p_list[i].id + 1);
                pthread_mutex_unlock(data->death_lock);
                return(0);
            }
        pthread_mutex_unlock(p_list[i].eating_lock);

            i++;
		}
		ft_usleep(1);
	}
    return 0;
}

void    thread_end(s_philodata *args, s_philo *philo, pthread_t *philo_t)
{
    int i;
    
    pthread_t *save = philo_t;
    i = 0;
    //pthread_join(args->supervisor, NULL);
    while (i < args->philo_num) {
        pthread_join(philo_t[i], NULL);
        i++;
    }
    i = 0;
    while (i < args->philo_num) 
    {
        pthread_mutex_destroy(philo[i].eating_lock);
        pthread_mutex_destroy(philo[i].left_fork);
        i++;
    }
    pthread_mutex_destroy(args->print_lock);
    pthread_mutex_destroy(args->death_lock);

    
    if(args->eating_lock)
        free(args->eating_lock);
    if(args->forks)
        free(args->forks);
    printf("aqui\n");
    if(save)
        free(save);
    if (philo)
        free(philo);
}

void    mutex_init(s_philodata *args, s_philo *philo)
{
    int i;
    pthread_mutex_t *forks;
    pthread_mutex_t *eating_lock;

    forks = malloc(args->philo_num * sizeof(pthread_mutex_t));
    eating_lock = malloc(args->philo_num * sizeof(pthread_mutex_t));
    args->forks = forks;
    args->eating_lock = eating_lock;
    i = 0;
    while (i < args->philo_num)
    {
        philo[i].left_fork = &forks[i];
        philo[i].eating_lock = &eating_lock[i];
		philo[i].right_fork = &forks[(i + 1) % args->philo_num];
        pthread_mutex_init(&forks[i], NULL);
        pthread_mutex_init(&eating_lock[i], NULL);
        i++;
    }
}

void    thread_init(s_philodata *args, pthread_t *philo_t, s_philo *philo)
{
    int i;
    pthread_mutex_t print_lock;
    pthread_mutex_t death_lock;
    //pthread_t supervisor;

    pthread_mutex_init(&death_lock, NULL);
    pthread_mutex_init(&print_lock, NULL);
    args->print_lock = &print_lock;
    args->death_lock = &death_lock;
    mutex_init(args, philo);
    //pthread_create(&supervisor, NULL, superv, args);
    //args->supervisor = supervisor;
    printf("%zu\n", get_current_time());
    i = 0;
    while (i < args->philo_num)
    {
        pthread_create(&philo_t[i], NULL, p_routine, &philo[i]);
        i++;
    }
}

void    philo_init(s_philodata *args)
{
    pthread_t *philo_t;
    s_philo *philo;
    int     i;
    
    philo_t = (pthread_t *)malloc(args->philo_num * sizeof( *philo_t));
    philo = malloc(args->philo_num * sizeof(s_philo));
    args->philo_thread = philo_t;
    args->philo_list = philo;
    args->start_time = get_current_time();
    i = 0;
    args->philosopher_died = 0;
    while (i < args->philo_num)
    {
        philo[i].id = i;
        philo[i].s_philodata = args;
        philo[i].last_meal = args->start_time;
        i++;
    }
    
    thread_init(args, philo_t, philo);
    thread_end(args, philo, philo_t);
}

int main(int argc, char *argv[]) 
{
    s_philodata args;

    if (argc < 5  || argc > 6)
    {
		printf("[philo_num] [time_to_die] [time_to_eat] [time_to_sleep]\n");
		return 1;
	}
    args.philo_num = atoi(argv[1]);
    args.time_to_die = atoi(argv[2]);
    args.time_to_eat = atoi(argv[3]);
    args.time_to_sleep = atoi(argv[4]);
    philo_init(&args);
    return 0;
}