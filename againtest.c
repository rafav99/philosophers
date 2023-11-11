

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

typedef struct t_dataget
{
	int				time_to_eat;
	int				time_to_sleep;
	int				philo_num;
	int				time_to_die;
	//int				times_eat;

} s_dataget;

int main(int argc, char *argv[]) {


    s_dataget args;

    args.philo_num = atoi(argv[1]);
    args.time_to_die = atoi(argv[2]);
    args.time_to_eat = atoi(argv[3]);
    args.time_to_sleep = atoi(argv[4]);
}
