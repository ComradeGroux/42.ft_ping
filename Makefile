BOLD	:= \033[1m
GRAY	:= \033[90m
GREEN	:= \033[32m
BLUE	:= \033[34m
RED		:= \033[31m
RESET	:= \033[0m
ERASE	:= \r\033[2K

NAME	= ft_ping

CC			= gcc
CFLAGS		= -Wall -Wextra -Werror -O3
DEBUG_FLAG  = -DDEBUG -g -fsanitize=address

RM			= rm -rf

INC_DIR		= include
SRC_DIR		= src

BUILD_DIR	= build
OBJ_DIR		= ${BUILD_DIR}/obj

SRCS_LIST 	=	main.c		\
				ping.c		\
				icmp.c		\
				socket.c	\
				print.c		\
				check.c

SRCS	:= ${addprefix ${SRC_DIR}/, ${SRCS_LIST}}
VPATH	:= $(dir $(SRCS))

OBJS	:= $(addprefix $(OBJ_DIR)/, $(notdir $(SRCS:.c=.o)))

all: ${NAME}

${NAME}: ${OBJ_DIR}/.compile_start ${OBJS}
	@printf "$(BOLD)Linking $(NAME)$(RESET)\n"
	@${CC} ${CFLAGS} ${OBJS} -o ${NAME} -lm
	@printf "$(GREEN)  ✓ $(NAME) ready$(RESET)\n"
	@printf "$(BOLD)Setting up capabilities (Need to be run as root)$(RESET)\n"
	@echo "sudo setcap ca_net_raw+ep"
	@sudo setcap cap_net_raw+ep $(NAME)
	@printf "$(GREEN)  ✓ capabilities (cap_net_raw + ep) set on $(NAME)$(RESET)\n"

${OBJ_DIR}/.compile_start: | ${OBJ_DIR}
	@printf "$(BOLD)Compiling $(NAME)$(RESET)\n"
	@touch $@


${OBJ_DIR}:
	@mkdir -p ${OBJ_DIR}

debug: CFLAGS += ${DEBUG_FLAG}
debug: re

${OBJ_DIR}/%.o: ${SRC_DIR}/%.c | ${OBJ_DIR}/.compile_start
	@printf "$(GRAY)  $<...$(RESET)" && \
	 ${CC} ${CFLAGS} -I${INC_DIR} -o $@ -c $< && \
	 printf "$(ERASE)$(GREEN)  ✓ $<$(RESET)\n"

clean:
	@printf "$(BOLD)$(BLUE)Cleaning $(NAME) objects...$(RESET)\n"
	@printf "$(GRAY)  Removing build objects...$(RESET)" && \
	 ${RM} ${OBJ_DIR} && \
	 printf "$(ERASE)"
	@printf "$(GREEN)  ✓ $(NAME) objects cleaned$(RESET)\n"

fclean: clean
	@printf "$(BOLD)$(BLUE)Cleaning $(NAME)...$(RESET)\n"
	@printf "$(GRAY)  Removing $(NAME)...$(RESET)" && \
	 ${RM} ${NAME} && \
	 printf "$(ERASE)"
	@printf "$(GREEN)  ✓ $(NAME) cleaned$(RESET)\n"

re: fclean all

.PHONY: all clean fclean re
