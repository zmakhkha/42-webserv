NAME		= WebServ
CC			= c++  -g -fsanitize=address
FLAGS		= -Wall -Wextra -Werror  -std=c++98 
OBJDIR 		= .obj

FILES		= 	Src/main \
				Src/ConfigFile/ConfigFile \
				Src/Server/Server \
				Src/Request/Request \
				Src/Response/Response \
				Src/Cgi/Cgi \

HEADER		=	Src/ConfigFile/ConfigFile.hpp \
				Src/ConfigFile/ConfigFile.tpp  \
				Src/Server/Server.hpp \
				Src/Request/Request.hpp \
				Src/Response/Response.hpp \
				Src/Cgi/Cgi.hpp

SRC			= $(FILES:=.cpp)
OBJ			= $(addprefix $(OBJDIR)/, $(FILES:=.o))


all: $(NAME)

$(NAME): $(OBJ) $(HEADER)
	@$(CC)  $(OBJ)   -o $(NAME) 
	@echo "🛰️  Server Ready!"

$(OBJDIR)/%.o: %.cpp $(HEADER)  Makefile
	@mkdir -p $(dir $@)
	@$(CC)  -g -c $< -o $@ 

clean: 
	@rm -rf $(OBJDIR) $(OBJ)
	@echo  "🗑️   Deleting OBJS."

fclean: clean
	@rm -rf  $(NAME)
	@echo  "🗑️   Deleting $(NAME)."

re: fclean all
del : 
	@rm /tmp/f*
	@rm /tmp/r*
.PHONY: all clean fclean re