
#include "philosophers.h"

void even_fork(s_philodata *data)
{
	size_t time;

	pthread_mutex_lock(data->left_fork);
	pthread_mutex_lock(data->printmt);
	time = get_current_time() - data->start_time;
	printf("%zu %d has taken a fork\n", time, data->id + 1);
	pthread_mutex_unlock(data->printmt);
	pthread_mutex_lock(data->right_fork);
	pthread_mutex_lock(data->printmt);
	time = get_current_time() - data->start_time;
	printf("%zu %d has taken a fork\n", time, data->id + 1);
	pthread_mutex_unlock(data->printmt);
}

void odd_fork(s_philodata *data)
{
	size_t time;

	pthread_mutex_lock(data->right_fork);
	pthread_mutex_lock(data->printmt);
	time = get_current_time() - data->start_time;
	printf("%zu %d has taken a fork\n", time, data->id + 1);
	pthread_mutex_unlock(data->printmt);
	pthread_mutex_lock(data->left_fork);
	pthread_mutex_lock(data->printmt);
	time = get_current_time() - data->start_time;
	printf("%zu %d has taken a fork\n", time, data->id + 1);
	pthread_mutex_unlock(data->printmt);
}

void	forkneat(s_philodata *data)
{
	size_t time;
	
	if(get_current_time()-data->start_time < 30)
	{
		if (data->id % 2 != 0)
			odd_fork(data);
		else
			even_fork(data);
	}
	else
			even_fork(data);

	pthread_mutex_lock(data->eating_lock);
	data->eating = 1;
	(data->myforks)[data->id] = 'b';
	pthread_mutex_lock(data->printmt);
	time = get_current_time() - data->start_time;
	printf("%zu %d is eating\n", time, data->id + 1);
	pthread_mutex_unlock(data->printmt);
	data->times_eat++;
	data->last_meal = get_current_time();
	ft_usleep(data->time_to_eat);
	(data->myforks)[data->id] = 'f';
	data->eating = 0;
	pthread_mutex_unlock(data->eating_lock);
	
	pthread_mutex_unlock(data->left_fork);
	pthread_mutex_unlock(data->right_fork);

}

void    thinknsleep(s_philodata *data)
{
    size_t  time;

    pthread_mutex_lock(data->printmt);
    time = get_current_time() - data->start_time;
    printf("%zu %d is sleeping\n", time,  data->id + 1);
    pthread_mutex_unlock(data->printmt);
    ft_usleep(data->time_to_sleep);
    pthread_mutex_lock(data->printmt);
    time = get_current_time() - data->start_time;
    printf("%zu %d is thinking\n",time, data->id + 1);
    pthread_mutex_unlock(data->printmt);
    if (data->philo_num % 2 != 0)
        ft_usleep((data->time_to_eat * 2)- data->time_to_sleep);
    else
        ft_usleep(((data->time_to_die- (data->time_to_eat + data->time_to_sleep))/2)-1);
}

void *routine(void *mydata)
{
	s_philodata *data;
	size_t time;

	data = (s_philodata *)mydata;
	while (1) 
	{
		forkneat(data);
		thinknsleep(data);
	}
	return NULL;
}