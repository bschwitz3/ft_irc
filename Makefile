NAME = Servers

CC =  c++

RM = 		@rm -rf


FLAGS = -g -Wall -Werror -Wextra -std=c++98 #-fsanitize=address #-pedantic 

CFILES =	main.cpp \
			srcs/User.cpp \
			srcs/Msg.cpp \
			srcs/Server.cpp \
			srcs/Command.cpp \
			srcs/Channel.cpp \
			srcs/RPL_ERR.cpp \



OBJECTS = 	$(CFILES:.cpp=.o)

$(NAME): 	$(OBJECTS)
			@$(CC) $(FLAGS) $(OBJECTS) -o $(NAME)

all: $(NAME)

clean:
	$(RM) $(OBJECTS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.cpp.o:
	${CC} ${FLAGS} -c $< -o ${<:.cpp=.o}

.PHONY: re fclean clean all
