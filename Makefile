#### NAME
NAME		= irc

#### SOURCES
SRC_PATH 	= srcs/
SRCS 		+= main.cpp
SRCS 		+= User.cpp
SRCS 		+= Channel.cpp
SRCS 		+= Server.cpp

vpath %.cpp $(SRC_PATH)

#### OBJECTS
PATH_OBJS	= objs/
OBJS		= $(patsubst %.cpp, $(PATH_OBJS)/%.o, $(SRCS))

#### HEADERS
INCLUDES 	+= includes/params.hpp
INCLUDES 	+= includes/Channel.hpp
INCLUDES 	+= includes/User.hpp
INCLUDES 	+= includes/Server.hpp


#### COMPILATION
COMP = c++
FLAGS += -Wall -Wextra -Werror
FLAGS += -std=c++98 -fsanitize=address
MAKEFLAGS += --no-print-directory

#### COLORS
BLUE = \033[1;94m
PURPLE = \033[1;95m
GREEN = \033[1;92m
YELLOW = \033[1;93m

#### RULES
all: $(NAME)


$(NAME): $(OBJS) $(INCLUDES) Makefile
	@$(COMP) $(FLAGS) $(OBJS) -o $(NAME) ./raph/TCP_IPv4.a
	@printf "$(YELLOW)------Compilation executed------\n\n"

$(OBJS): $(PATH_OBJS)/%.o: %.cpp $(INCLUDES)
	@mkdir -p $(PATH_OBJS)
	@$(COMP) $(FLAGS) -c $< -o $@ -I./includes

clean:
	@$(RM) -R $(PATH_OBJS)
	@printf "$(PURPLE)------Object files deleted-------\n\n"

fclean: clean
	@$(RM) $(NAME)
	@printf "$(GREEN)----Executable files deleted-----\n\n"

re: fclean all

.PHONY : all clean fclean re