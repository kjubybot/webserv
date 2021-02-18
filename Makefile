NAME = webserv

OBJDIR = objs/
SRCDIR = srcs/

OBJ =\
	main.o\
	Server.o\
	Host.o\
	Connection.o\
	Request.o\
	Response.o\
	HttpErrorPage.o\
	HttpErrorException.o\
	CGI.o\
	Config.o\
	util.o
OBJ := $(addprefix $(OBJDIR),$(OBJ))

NB = $(words $(OBJ))
INDEX = 0

CXX = clang++
CXXFLAGS = -MMD -Wall -Wextra -Werror -std=c++98 -Iincludes/

all: $(OBJDIR) $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)
	@printf "\r\033[38;5;46mDONE\033[0m\033[K\n"

$(OBJDIR)%.o: $(SRCDIR)%.cpp
	@$(CXX) $(CXXFLAGS) -c -o $@ $<
	@$(eval PERCENT=$(shell expr $(INDEX) '*' 100 / $(NB)))
	@$(eval PROGRESS=$(shell expr $(INDEX) '*' 30 / $(NB)))
	@printf "\r\033[38;5;220mMAKE %2d%%\033[0m \033[48;5;220m%*s\033[0m %s\033[K" $(PERCENT) $(PROGRESS) "" $(notdir $@)
	@$(eval INDEX=$(shell echo $$(($(INDEX)+1))))

$(OBJDIR):
	@mkdir -p $(OBJDIR)

clean:
	@$(RM) -r $(OBJDIR)
	@printf "\033[38;5;124mCLEAN\033[0m\033[K\n"

fclean: clean
	@$(RM) $(NAME)
	@printf "\033[38;5;160mFCLEAN\033[0m\033[K\n"

re: fclean all

-include $(OBJ:.o=.d)
.PHONY: all clean fclean re