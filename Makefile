CPP			= c++
CPPFLAGS	= -Wall -Werror -Wextra -std=c++98 -pedantic
RM			= rm -f
NAME		= webserv
SRC			= main.cpp
OBJ			= $(SRC:.cpp=.o)

all:		$(NAME)

$(NAME):	$(OBJ)
			$(CPP) $(OBJ) $(CPPFLAGS) -o $@

%.o:		%.cpp
			$(CPP) -c $^ $(CPPFLAGS) -o $@

clean:
			$(RM) $(OBJ)

fclean:		clean
			$(RM) $(NAME)

re:			fclean all

.PHONY:		all clean fclean re