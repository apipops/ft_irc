#------------------------------------#
#               Project              #
#------------------------------------#
TARGET		=	ft_irc
PROJDIR		=	$(realpath $(CURDIR))
SRCDIR		=	$(PROJDIR)/srcs
INCDIR		=	$(PROJDIR)/includes
OBJDIR		=	$(PROJDIR)/objs
DEPDIR		=	$(PROJDIR)/.deps

#------------------------------------#
#               Compiler             #
#------------------------------------#
CC			=	c++
CFLAGS		=	-Wall -Wextra -Werror -std=c++98
INCLUDE		=	-I $(INCDIR) -I $(PROJDIR)/raph

#------------------------------------#
#                Files               #
#------------------------------------#
SRCS		=	$(shell find $(SRCDIR) -type f -name '*'.cpp)
OBJS		=	$(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SRCS:.cpp=.o))
DEPS		=	$(patsubst $(SRCDIR)/%,$(DEPDIR)/%,$(SRCS:.cpp=.d))
LIB			=	$(PROJDIR)/raph/TCP_IPv4.a

#------------------------------------#
#                Rules               #
#------------------------------------#
all : $(TARGET)

$(TARGET) : $(LIB) $(OBJS)
	@$(CC) $(CFLAGS) $(INCLUDE) $^ -o $@ $(LIB)
	@echo Linking $@

$(LIB) :
	@make verbose -C ./raph

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	@mkdir -p $(patsubst $(OBJDIR)/%,$(DEPDIR)/%,$(dir $@))
	@$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@ -MMD -MF $(patsubst $(OBJDIR)/%,$(DEPDIR)/%,$(@:.o=.d))
	@echo Building $(notdir $@)

-include $(DEPS)

clean :
	make clean -C ./raph
	@rm -rf $(OBJDIR)
	@rm -rf $(DEPDIR)
	@echo Cleaning $(OBJDIR) $(DEPDIR)

fclean : clean
	@make fclean -C ./raph
	@rm -f $(TARGET)
	@echo Cleaning $(TARGET)

re : fclean all

.PHONY : all clean fclean re