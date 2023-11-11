#ifndef PHILOSOPHERS_H

# define PHILOSOPHERS_H

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>


typedef struct t_philodata
{
	int				id;
	int				time_to_eat;
	int				time_to_sleep;
	int				philo_num;
	int				time_to_die;
	int				eating;
	int				option;
	int				times_eat;
	char			*myforks;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	pthread_mutex_t	*printmt;
	pthread_mutex_t	*eating_lock;
	pthread_mutex_t	*option_lock;
	size_t			start_time;
	size_t			last_meal;

} s_philodata;

void	even_fork(s_philodata *data);
void	odd_fork(s_philodata *data);
void	forkneat(s_philodata *data);
void    thinknsleep(s_philodata *data);
void	*routine(void *mydata);
void	data_setup(	s_philodata *data, int philo_num);
void	thread_init(s_philodata *data, int philo_num);
void	data_setup(s_philodata *data, int philo_num);
void	asign_thread(s_philodata *data, pthread_mutex_t printmt, pthread_t *my_philos, pthread_t supervisor);
void	data_end(s_philodata *data, pthread_t *my_philos, pthread_mutex_t *forks);
void	check_death(s_philodata *data);
void	time_eaten(s_philodata *data, int *counter, int eat_counter, int i);
void	ft_exit(s_philodata *data);
int		ft_usleep(size_t ms);
size_t	get_current_time(void);

#endif