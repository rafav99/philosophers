/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   all.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafav99 <rafav99@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 04:26:47 by rafav99           #+#    #+#             */
/*   Updated: 2023/11/10 14:27:26 by rafav99          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"



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


void	ft_exit(s_philodata *data)
{
	int	i;

	i = 0;
	while (i < data->philo_num)
	{
		pthread_mutex_destroy(data[i].left_fork);
		pthread_mutex_destroy(data[i].eating_lock);
		i++;
	}
	pthread_mutex_destroy(data[0].printmt);
}

void	time_eaten(s_philodata *data, int *counter, int eat_counter, int i)
{
	size_t current_time;

	/*if (get_current_time() - data[i].last_meal > data[i].time_to_die)
	{
		current_time = get_current_time() - data[i].start_time;
		printf("%zu %d died\n", current_time,  data[i].id + 1);
		exit(1);
	}*/

	if (data[0].option > 0)
	{
		if(data[i].times_eat == eat_counter)
		{
			*counter = *counter + 1;
			data[i].times_eat = data[i].times_eat + 2;
		}
		if(*counter == data[0].philo_num)
			exit(1);

	}
}
void	check_death(s_philodata *data)
{
	int i;
	int		counter;
	int		eat_counter;

	eat_counter = data[0].option;
	counter = 0;
	while (1)
	{
		i = 0;
		while (i < data[0].philo_num) 
		{
			time_eaten(data, &counter, eat_counter, i);
			i++;
		}
		ft_usleep(1);
	}
}

void	data_end(s_philodata *data, pthread_t *my_philos, pthread_mutex_t *forks)
{
	int j = 0;
	while (j < data[0].philo_num) 
	{
		pthread_join(my_philos[j], NULL);
		pthread_mutex_destroy(data[j].eating_lock);
		pthread_mutex_destroy(&forks[j]);
		j++;
	}	
	pthread_mutex_destroy(data[0].printmt);
}	

void	asign_thread(s_philodata *data, pthread_mutex_t printmt, pthread_t *my_philos, pthread_t supervisor)
{
	int	i;
	pthread_mutex_t *forks;
	pthread_mutex_t *eating_lock;

	forks = malloc(data[0].philo_num * sizeof(pthread_mutex_t));
	eating_lock = malloc(data[0].philo_num * sizeof(pthread_mutex_t));
	i = 0;
	while (i < data[0].philo_num) 
	{
		data[i].id = i;
		data[i].left_fork = &forks[i];
		data[i].right_fork = &forks[(i + 1) % data[0].philo_num];
		data[i].printmt = &printmt;
		data[i].eating_lock = &eating_lock[i];
		pthread_mutex_init(&eating_lock[i], NULL);
		pthread_mutex_init(&forks[i], NULL);
		pthread_create(&my_philos[i], NULL, routine, &data[i]);
		i++;
	}
	data_end(data, my_philos, forks);
	//pthread_join(supervisor, NULL);

	free(my_philos);
	free(data[0].printmt);
	free(data);
	free(forks);

}

void	thread_init(s_philodata *data, int philo_num)
{
	pthread_t *my_philos;
	pthread_mutex_t printmt;
	pthread_t supervisor;
	int i;
	
	my_philos = malloc(philo_num * sizeof(pthread_t));
    pthread_mutex_init(&printmt, NULL);
	//pthread_create(&supervisor, NULL, (void *)check_death, data);
	asign_thread(data, printmt, my_philos, supervisor);
}

void	data_setup(s_philodata *data, int philo_num)
{
	int	i;
	size_t start_time;
	pthread_mutex_t *option_lock;
	char *myforks;

	myforks = malloc(philo_num);
	if (data[0].option > 0)
		option_lock = malloc(sizeof(pthread_mutex_t));
	start_time = get_current_time();
	data[0].last_meal = start_time;
	i = 0;
	while(i < philo_num)
	{
		data[i].time_to_die = data[0].time_to_die;
		data[i].last_meal = data[0].last_meal;
		data[i].start_time = start_time;
		data[i].eating = 0;
		data[i].philo_num = philo_num;
		data[i].time_to_eat = data[0].time_to_eat;
		data[i].time_to_sleep = data[0].time_to_sleep;
		data[i].times_eat = 0;
		data[i].option_lock = option_lock;
		myforks[i] = 'f';
		data[i].myforks = myforks;
		i++;
	}
	thread_init(data, philo_num);
}



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
	Dpthread_mutex_unlock(data->eating_lock);

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
int main(int argc, char *argv[]) 
{
	s_philodata *data;
	int philo_num;
	int i;
	int	option;

	if (argc < 5  || argc > 6) {
		printf("[philo_num] [time_to_die] [time_to_eat] [time_to_sleep]\n");
		return 1;
	}
	philo_num = atoi(argv[1]);
	data = malloc(philo_num * sizeof(s_philodata));	
	if (argc != 6)
		option = -1;
	else
		data[0].option = atoi(argv[5]);
	data[0].time_to_die = atoi(argv[2]);
	data[0].time_to_eat = atoi(argv[3]);
	data[0].time_to_sleep = atoi(argv[4]);
	data_setup(data, philo_num);
	
	return 0;
}

