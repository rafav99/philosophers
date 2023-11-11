#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>

// Gets the current time in milliseconds

size_t	get_current_time(void)
{
	struct timeval	time;

	if (gettimeofday(&time, NULL) == -1)
		write(2, "gettimeofday() error\n", 22);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
}

typedef struct	s_philo
{
	pthread_t		thread;
	int	philo_num;
	int philo_id;
	int	time_to_eat;
	int	time_to_die;
	int time_to_sleep;
	int num_of_times;
	size_t start_time;
	pthread_mutex_t	*r_fork;
	pthread_mutex_t	*l_fork;
	char *philo_list;
}					t_philo;

int	isnumber(char *arg)
{
	int	i;
	
	i = 0;
	while (arg[i] != '\0')
	{
		if (arg[i] < '0' || arg[i] > '9')
			return(0);
		i++;
	}
	return (1);
}
int	ft_atoi(const char *nptr)
{
	long	number;
	int		negative;
	int		i;

	number = 0;
	negative = 1;
	i = 0;
	while ((nptr[i] == ' ') || (nptr[i] >= 9 && nptr[i] <= 13))
		i++;
	if (nptr[i] == '-' || nptr[i] == '+')
	{
		if (nptr[i] == '-')
			negative = -negative;
		i++;
	}
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		number = number * 10 + nptr[i] - 48;
		if ((number * negative) < -2147483648)
			return (0);
		if ((number * negative) > 2147483647)
			return (-1);
		i++;
	}
	return (number * negative); 
}

void *p_routine(void *philo_stats)
{
	size_t tstamp;
	size_t start_time = get_current_time();
	
	while(1)
	{ 
		tstamp = get_current_time() - start_time;
		printf("%zu %i has taken a fork\n", tstamp, ((t_philo *)philo_stats)->philo_id);
		printf("%zu %i is eating\n", tstamp, ((t_philo *)philo_stats)->philo_id);
		usleep(((t_philo *)philo_stats)->time_to_eat * 1000);
		tstamp = get_current_time() - start_time;
		printf("%zu %i is sleeping\n", tstamp, ((t_philo *)philo_stats)->philo_id);
		usleep(((t_philo *)philo_stats)->time_to_sleep);
		tstamp = get_current_time() - start_time;
		printf("%zu %i is thinking\n", tstamp, ((t_philo *)philo_stats)->philo_id);
		usleep(((t_philo *)philo_stats)->time_to_die);
		tstamp = get_current_time() - start_time;
		printf ("%zu %i died\n", tstamp,((t_philo *)philo_stats)->philo_id);
		return(philo_stats);
	}
	
}

int main(int argc, char *argv[])
{	
	int	i;
	
	t_philo *philo_stats;
	char *forks;

	if (argc > 6 || argc < 5)
	{
		exit(2);
	}
	i = 1;
	while(i < argc)
	{
		if (isnumber(argv[i]) == 1)
			i++;
		else
			exit(2);
	}	
	philo_stats = malloc(ft_atoi(argv[4])* sizeof(t_philo));
	forks = malloc(ft_atoi(argv[4]));
	i = 0;
	while (i < ft_atoi(argv[1]))
	{
		philo_stats[i].philo_id = i;

		philo_stats[i].time_to_die = ft_atoi(argv[2]);
		philo_stats[i].time_to_eat = ft_atoi(argv[3]);
		philo_stats[i].time_to_sleep = ft_atoi(argv[4]);
		philo_stats[i].philo_num = ft_atoi(argv[1]);
		pthread_mutex_init(philo_stats[i].r_fork, NULL);
		if (i > 0)
			philo_stats[i].l_fork = philo_stats[i].r_fork;
		else
			philo_stats[i].l_fork = philo_stats[ft_atoi(argv[1] - 1)].r_fork;
		pthread_create(&(philo_stats[i].thread), NULL, p_routine, (void*) &philo_stats[i]);
		i++;
	}
	i = 0;
	while (i < ft_atoi(argv[1]))
	{
		philo_stats[i].philo_id = i;
		pthread_join(philo_stats[i].thread, NULL);
		i++;
	}
	free(philo_stats);
}
