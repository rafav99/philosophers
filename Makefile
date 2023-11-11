NAME = philosophers

SRC = philosophers.c routine.c
OBJ = $(SRC:%.c=%.o)

CC = clang
CFLAGS = -pthread -fsanitize=thread 

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	
clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re