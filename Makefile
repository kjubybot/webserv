NAME = webserv
SRC = $(addprefix srcs/,\
	main.cpp\
	Server.cpp\
	Host.cpp\
	Connection.cpp\
	Request.cpp\
	Response.cpp\
	HttpErrorPage.cpp\
	HttpErrorException.cpp\
	CGI.cpp\
	Config.cpp\
	util.cpp)

OBJ = $(SRC:.cpp=.o)

CXX = clang++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -Iincludes/

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re