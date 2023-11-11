/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philotest.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafav99 <rafav99@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 03:45:48 by rafav99           #+#    #+#             */
/*   Updated: 2023/11/11 02:35:35 by rafav99          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOTEST_H

# define PHILOTEST_H

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

struct  t_philo;
typedef struct t_philodata
{
	int				time_to_eat;
	int				time_to_sleep;
	int				philo_num;
	int				time_to_die;
    int             philosopher_died;
    size_t          start_time;
    pthread_mutex_t *forks;
    pthread_mutex_t *eating_lock;
    pthread_mutex_t *print_lock;
    pthread_mutex_t *death_lock;
    pthread_t       supervisor;
    pthread_t       *philo_thread;
    struct  t_philo *philo_list;

} s_philodata;

typedef struct t_philo
{
    int             id;
    size_t             last_meal;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
    pthread_mutex_t *eating_lock;
    s_philodata     *s_philodata;
} s_philo;










#endif