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
	int	philo_num;
	int philo_id;
	int	time_to_eat;
	int	time_to_die;
	int time_to_sleep;
	int num_of_times;
	size_t start_time;
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

	tstamp = get_current_time() - ((t_philo *)philo_stats)->start_time;
	while(1)
	{
		printf("%i\n", ((t_philo *)philo_stats)->philo_id);
		tstamp = get_current_time() - ((t_philo *)philo_stats)->start_time;
		printf("%zu has taken a fork\n", tstamp);
		printf("%zu is eating\n", tstamp);
		usleep(10000);
		usleep(((t_philo *)philo_stats)->time_to_eat);
		printf(" time is %zu\n", get_current_time());
		tstamp = get_current_time() - ((t_philo *)philo_stats)->start_time;
		printf("%zu is sleeping\n", tstamp);
		usleep(((t_philo *)philo_stats)->time_to_sleep);
		tstamp = get_current_time() - ((t_philo *)philo_stats)->start_time;
		printf("%zu is thinking\n", tstamp);
		//printf ("%zu died\n", tstamp);
	return(philo_stats);
	}
	
}

int main(int argc, char *argv[])
{	
	int	i;
	
	t_philo philo_stats; 
	pthread_t philo1;
  	pthread_t philo2;
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
	philo_stats.philo_num = ft_atoi(argv[1]);
	philo_stats.time_to_eat = ft_atoi(argv[2]);
	philo_stats.time_to_sleep = ft_atoi(argv[3]);
	philo_stats.philo_num = ft_atoi(argv[4]);
	philo_stats.philo_list = malloc(philo_stats.philo_num);
	philo_stats.start_time = get_current_time();
	printf("start time is %zu\n", philo_stats.start_time);
	philo_stats.philo_id = 1;
	pthread_create(&philo1, NULL, p_routine, (void*) &philo_stats);
	philo_stats.philo_id = 2;
	pthread_create(&philo2, NULL, p_routine, (void*) &philo_stats);
	pthread_join(philo1, NULL);
  	pthread_join(philo2, NULL);
}