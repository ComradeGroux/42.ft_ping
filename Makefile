GREENGREEN = \033[38;5;46m
RED = \033[0;31m
GREY = \033[38;5;240m
RESET = \033[0m

NAME	= ft_ping

CC			= gcc
CFLAGS		= -Wall -Wextra -Werror -O3
DEBUG_FLAG  = -DDEBUG -g -fsanitize=address

RM			= rm -rf

INC_DIR		= include
SRC_DIR		= src

BUILD_DIR	= build
OBJ_DIR		= ${BUILD_DIR}/obj

SRCS_LIST 	=	main.c	\
				ping.c

SRCS	:= ${addprefix ${SRC_DIR}/, ${SRCS_LIST}}
VPATH	:= $(dir $(SRCS))

OBJS	:= $(addprefix $(OBJ_DIR)/, $(notdir $(SRCS:.c=.o)))


${NAME}: ${BUILD_DIR} ${OBJS}
	@echo "$(RESET)[$(GREENGREEN)${NAME}$(RESET)]: ${NAME} Objects were created${GREY}"
	${CC} ${CFLAGS} ${OBJS} -o ${NAME}
	@echo "$(RESET)[$(GREENGREEN)${NAME}$(RESET)]: ${NAME} created !"

${BUILD_DIR}:
	mkdir -p ${BUILD_DIR}
	mkdir -p ${OBJ_DIR}

all: ${NAME}

debug: CFLAGS += ${DEBUG_FLAG}
debug: re

${OBJ_DIR}%.o:${SRC_DIR}%.c
	@printf "\033[38;5;240m"
	${CC} ${CFLAGS} ${LIBFT_INC} -I${INC_DIR} -o $@ -c $<

clean:
	@echo "[$(RED)${NAME}$(RESET)]: Cleaning ${NAME} Objects...${GREY}"
	${RM} ${OBJ_DIR}
	@echo "[$(RED)${NAME}$(RESET)]: ${NAME} Objects were cleaned${GREY}"

fclean: clean
	@echo "${RESET}[$(RED)${NAME}$(RESET)]: Cleaning ${NAME}...${GREY}"
	${RM} ${NAME}
	${RM} ${BUILD_DIR}
	@echo "${RESET}[$(RED)${NAME}$(RESET)]: ${NAME} was cleaned"

re: fclean all

.PHONY: all clean fclean re
