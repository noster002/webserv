CPP				= c++
CPPFLAGS		= -Wall -Werror -Wextra -std=c++98 -pedantic -I$(HEADDIR)
RM				= rm -f
NAME			= webserv
SRCDIR			= srcs
OBJDIR			= objs
HEADDIR			= head
SRC				= $(addprefix $(SRCDIR)/,$(addsuffix .cpp,\
				  main Server Config Error Client Socket Request Response))
OBJ				= $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC))
HEAD			= $(addprefix $(HEADDIR)/,$(addsuffix .hpp,\
				  Server Config Error Client Socket Helpers Request Response))

all:			$(NAME)

$(NAME):		$(OBJ)
				$(CPP) $(OBJ) $(CPPFLAGS) -o $@

$(OBJDIR)/%.o:	$(SRCDIR)/%.cpp
				$(CPP) -c $^ $(CPPFLAGS) -o $@

clean:
				$(RM) $(OBJ)

fclean:			clean
				$(RM) $(NAME)

re:				fclean all

.PHONY:			all clean fclean re
