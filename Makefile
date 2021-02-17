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
	@printf "\r\e[38;5;46mDONE\e[0m\e[K\n"

$(OBJDIR)%.o: $(SRCDIR)%.cpp
	@$(CXX) $(CXXFLAGS) -c -o $@ $<
	@$(eval PERCENT=$(shell echo $$(($(INDEX)*100/$(NB)))))
	@printf "\r\e[38;5;226mMAKE %2d%%\e[0m %s\e[K" $(PERCENT) $@
	@$(eval INDEX=$(shell echo $$(($(INDEX)+1))))

$(OBJDIR):
	@mkdir -p $(OBJDIR)

clean:
	@$(RM) -r $(OBJDIR)
	@printf "\e[38;5;124mCLEAN\e[0m\e[K\n"

fclean: clean
	@$(RM) $(NAME)
	@printf "\e[38;5;160mFCLEAN\e[0m\e[K\n"

re: fclean all

-include $(OBJ:.o=.d)
.PHONY: all clean fclean re