/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   routines.c									 :+:	  :+:	:+:   */
/*													+:+ +:+		 +:+	 */
/*   By: rvaldes- <rvaldes-@student.42.fr>		  +#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2023/11/08 18:10:15 by rvaldes-		  #+#	#+#			 */
/*   Updated: 2023/11/08 18:12:46 by rvaldes-		 ###   ########.fr	   */
/*																			*/
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

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

typedef struct t_philodata
{
	int				id;
	int				time_to_eat;
	int				time_to_sleep;
	int				philo_num;
	int				time_to_die;
	int				eating;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	pthread_mutex_t	*printmt;
	pthread_mutex_t	*eating_lock;
	size_t			start_time;
	size_t			last_meal;

} s_philodata;

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

	if (data->id % 2 == 0)
		odd_fork(data);
	else
		even_fork(data);
	pthread_mutex_lock(data->eating_lock);
	data->eating = 1;
	pthread_mutex_lock(data->printmt);
	time = get_current_time() - data->start_time;
	printf("%zu %d is eating\n", time, data->id + 1);
	pthread_mutex_unlock(data->printmt);
	ft_usleep(data->time_to_eat);
	data->last_meal = get_current_time();
	pthread_mutex_unlock(data->eating_lock);
	pthread_mutex_unlock(data->left_fork);
	pthread_mutex_unlock(data->right_fork);
	data->eating = 0;

}
void *routine(void *mydata)
{
	s_philodata *data;
	size_t time;

	data = (s_philodata *)mydata;
	while (1) 
	{
		forkneat(data);
		pthread_mutex_lock(data->printmt);
		time = get_current_time() - data->start_time;
		printf("%zu %d is sleeping\n", time,  data->id + 1);
		pthread_mutex_unlock(data->printmt);
		ft_usleep(data->time_to_sleep);
		pthread_mutex_lock(data->printmt);
		time = get_current_time() - data->start_time;
		printf("%zu %d is thinking\n",time, data->id + 1);
		pthread_mutex_unlock(data->printmt);
	}
	return NULL;
}

void	check_death(s_philodata *data)
{
	int i;
	size_t time;
	size_t current_time;
	while (1)
	{
		i = 0;
		while (i < data[0].philo_num) 
		{
			time = get_current_time();
			if (time - data[i].last_meal > data[i].time_to_die && data[i].eating != 1)
			{
				pthread_mutex_lock(data[i].eating_lock);
				current_time = get_current_time() - data[i].start_time;
				printf("%zu %d died: %zu\n", current_time,  data[i].id + 1, data[i].last_meal - data[i].start_time);
				pthread_mutex_unlock(data[i].eating_lock);
				exit(1);
			}
			i++;
		}
		ft_usleep(1);
	}
}

void	data_init(s_philodata *data, int philo_num)
{
	pthread_t *routines = malloc(philo_num * sizeof(pthread_t));
	pthread_mutex_t *forks = malloc(philo_num * sizeof(pthread_mutex_t));
	pthread_mutex_t *printmt = (pthread_mutex_t *)malloc( sizeof(pthread_mutex_t));
	pthread_mutex_t *eating_lock = (pthread_mutex_t *)malloc(philo_num * sizeof(pthread_mutex_t));
	pthread_t supervisor;
	pthread_create(&supervisor, NULL, (void *)check_death, data);
	int i = 0;

	while (i < philo_num) {
		data[i].id = i;
		data[i].left_fork = &forks[i];
		data[i].right_fork = &forks[(i + 1) % philo_num];
		data[i].printmt = printmt;
		data[i].eating_lock = &eating_lock[i];
		data[i].start_time = get_current_time();
		pthread_mutex_init(&eating_lock[i], NULL);
		pthread_mutex_init(&forks[i], NULL);
		pthread_create(&routines[i], NULL, routine, &data[i]);
		i++;
	}
	int j = 0;
	while (j < philo_num) 
	{
		pthread_join(routines[j], NULL);
		pthread_mutex_destroy(&forks[j]);
		j++;
	}	pthread_join(supervisor, NULL);

	pthread_mutex_destroy(eating_lock);
	pthread_mutex_destroy(printmt);
	free(routines);
	free(data);
	free(forks);
}

void	data_setup(	s_philodata *data, int philo_num)
{
	int	i;

	size_t start_time;

	start_time = get_current_time();
	data[0].last_meal = start_time;
	i = 0;
	while(i < philo_num)
	{
		data[i].time_to_die = data[0].time_to_die;
		data[i].last_meal = data[0].last_meal;
		data[i].eating = 0;
		data[i].philo_num = philo_num;
		data[i].time_to_eat = data[0].time_to_eat;
		data[i].time_to_sleep = data[0].time_to_sleep;
		i++;
	}
	data_init(data, philo_num);
}

int main(int argc, char *argv[]) 
{
	s_philodata *data;
	int philo_num;
	int i;

	if (argc < 4) {
		fprintf(stderr, "Usage: %s <number_of_routines> <time_to_eat>\n", argv[0]);
		return 1;
	}
	philo_num = atoi(argv[1]);
	data = malloc(philo_num * sizeof(s_philodata));	
	data[0].time_to_die = atoi(argv[2]);
	data[0].time_to_eat = atoi(argv[3]);
	data[0].time_to_sleep = atoi(argv[4]);
	data_setup(data, philo_num);
	
	return 0;
}
