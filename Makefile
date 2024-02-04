NAME		= webserv
CC			= c++ -g
FLAGS		= -Wall -Wextra -Werror  -std=c++98
OBJDIR 		= .obj

FILES		= 	Src/main \
				Src/ConfigFile/myconfig \
				Src/Server/Server \
				Src/Request/Request \
				Src/Response/Response \
				Src/Cgi/Cgi \

HEADER		=	Src/ConfigFile/myconfig.hpp \
				Src/ConfigFile/myconfig.tpp  \
				Src/Server/Server.hpp \
				Src/Request/Request.hpp \
				Src/Response/Response.hpp \
				Src/Cgi/Cgi.hpp

SRC			= $(FILES:=.cpp)
OBJ			= $(addprefix $(OBJDIR)/, $(FILES:=.o))


all: $(NAME)

$(NAME): $(OBJ) $(HEADER)
	@$(CC) $(FLAGS) $(OBJ)   -o $(NAME) 
	@echo "🛰️  Server Ready!"

$(OBJDIR)/%.o: %.cpp $(HEADER)  Makefile
	@mkdir -p $(dir $@)
	@$(CC) $(FLAGS) -g -c $< -o $@ 

clean: 
	@Src/remove
	@rm -rf $(OBJDIR) $(OBJ)
	@echo  "🗑️   Deleting OBJS."

fclean: clean
	@rm -rf  $(NAME)
	@echo  "🗑️   Deleting $(NAME)."

re: fclean all

.PHONY: all clean fclean re