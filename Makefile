
##__________CONFIG__________#

NAME		=	ft_ping

CC			=	gcc
CFLAGS		+=	-fPIC -Wall -Werror -Wextra -O3

INC_PATH		=	./includes/
INCLUDES	=	$(NAME).h	\
INCS		=	$(addprefix $(INC_PATH), $(INCLUDES))

SRC_PATH	=	./srcs/
SRC			=	$(NAME).c	\
				socket.c \
				packet.c \
				utils.c \
				verbose.c

BIN_PATH	=	./bins/
BIN			=	$(SRC:.c=.o)
BINS		=	$(addprefix $(BIN_PATH), $(BIN))

.PHONY: all clean fclean re

#__________RULES__________#

all: $(NAME)

$(NAME): $(BINS)
	
	@$(CC) $(CFLAGS) -o $@ $^ -I $(INC_PATH)
	@echo "\n[EXECUTABLE \"$(NAME)\" READY]\n"

$(BIN_PATH)%.o: $(SRC_PATH)%.c $(INCS)

	@mkdir -p $(BIN_PATH) || true
	@$(CC) $(CFLAGS) -I $(INC_PATH) -o $@ -c $< && echo " \c"

clean:
	@rm -Rf $(BIN_PATH)
	@echo "\n[CLEANING $(NAME) BINARIES]"

fclean: clean
	@rm -f $(NAME)
	@echo "\n[REMOVING \"$(NAME)\"]"

re: fclean all
