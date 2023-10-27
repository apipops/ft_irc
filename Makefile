#### NAME
NAME		= irc

#### SOURCES
SRC_PATH 	= srcs/
SRCS 		+= main.cpp
SRCS 		+= User.cpp
SRCS 		+= Channel.cpp
# SRCS 		+= Message.cpp
SRCS 		+= IRCServer.cpp

vpath %.cpp $(SRC_PATH)

#### OBJECTS
PATH_OBJS	= objs/
OBJS		= $(patsubst %.cpp, $(PATH_OBJS)/%.o, $(SRCS))

#### HEADERS
INCLUDES 	+= includes/params.hpp
INCLUDES 	+= includes/Channel.hpp
INCLUDES 	+= includes/User.hpp
# INCLUDES 	+= includes/Message.hpp
INCLUDES 	+= includes/IRCServer.hpp

#### LIB RAPH
PATH_RAPH	+= raph

#### COMPILATION
COMP = c++
FLAGS += -Wall -Wextra -Werror
FLAGS += -std=c++98 
FLAGS += -fsanitize=address
MAKEFLAGS += --no-print-directory

#### COLORS
BLUE = \033[1;94m
PURPLE = \033[1;95m
GREEN = \033[1;92m
YELLOW = \033[1;93m
RED= \033[1;31m
DEFAULT=\033[1;39m

#### RULES
all: raph $(NAME)


$(NAME): $(OBJS) $(INCLUDES) Makefile
	@$(COMP) $(FLAGS) $(OBJS) -o $(NAME) ./raph/TCP_IPv4.a
	@printf "$(YELLOW)-------------COMPILATION EXECUTED--------------$(DEFAULT)\n\n"

$(OBJS): $(PATH_OBJS)/%.o: %.cpp $(INCLUDES)
	@mkdir -p $(PATH_OBJS)
	@$(COMP) $(FLAGS) -c $< -o $@ -I./includes

raph:
	@make -C $(PATH_RAPH)
	@printf "$(RED)------------LIB 'TCP_IPv4' CREATED-------------$(DEFAULT)\n\n"

clean:
	@$(RM) -R $(PATH_OBJS) server.log
	@make clean -C $(PATH_RAPH)
	@printf "$(PURPLE)---------------OBJECT FILES DELETED------------$(DEFAULT)\n\n"

fclean: clean
	@$(RM) $(NAME)
	@make fclean -C $(PATH_RAPH)
	@printf "$(GREEN)-------------EXECUTABLE FILES DELETED----------$(DEFAULT)\n\n"

re: fclean all

.PHONY : all clean fclean re raph