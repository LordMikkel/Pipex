# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: migarrid <migarrid@student.42barcelona.    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/19 17:55:34 by migarrid          #+#    #+#              #
#    Updated: 2025/07/02 01:45:30 by migarrid         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# **************************************************************************** #
#                              Project Information                             #
# **************************************************************************** #
NAME				= pipex
NAME_BONUS			= pipex_bonus

# **************************************************************************** #
#                            Compiler and Flags                                #
# **************************************************************************** #
CC					= cc
CFLAGS				= -g #-Wall -Wextra -Werror #-fsanitize=address,undefined -O0

# **************************************************************************** #
#                               Shell Comands                                  #
# **************************************************************************** #
RM					= rm -rf
PRINT				= printf
MAKE				= make
MKDIR				= mkdir -p
NORM				= norminette

# **************************************************************************** #
#                              Directories                                     #
# **************************************************************************** #
INC_DIR				= inc
LIB_DIR				= lib
MAPS_DIR			= maps
OBJ_DIR				= obj
OBJ_BONUS_DIR		= $(OBJ_DIR)/bonus
SRC_DIR				= src
SRC_BONUS_DIR 		= $(SRC_DIR)/bonus
LIBFT_DIR			= $(LIB_DIR)
LIBX				= $(LIB_DIR)/XXXXX
DEPS				= $(HEADER) $(MAKEFILE) $(LIBFT_H) $(LIBFT_MAKEFILE)

# **************************************************************************** #
#                      File Paths and Dependencies                             #
# **************************************************************************** #
MAKEFILE				= Makefile
HEADER					= $(INC_DIR)/pipex.h
LIBFT_A					= $(LIBFT_DIR)/libft_plus.a
LIBFT_H					= $(LIBFT_DIR)/libft_plus.h
LIBFT_MAKEFILE			= $(LIBFT_DIR)/Makefile

# **************************************************************************** #
#                                   Colors                                     #
# **************************************************************************** #
DEFAULT				= \033[0;39m
GREEN 				= \033[0;92m
BLUE 				= \033[0;94m
MAGENTA 			= \033[0;95m
CYAN 				= \033[0;96m
YELLOW 				= \033[0;33m
RED 				= \033[0;31m
GREY 				= \033[38;5;240m
PURPLE 				= \033[38;5;141m
RESET 				= \033[0m
BOLD 				= \033[1m
CLEAR 				= \r\033[K

# **************************************************************************** #
#                               Source File                                    #
# **************************************************************************** #
SRCS =				pipex.c \
					parent.c \
					child.c \
					path.c \
					init.c \
					clean.c \
					open.c \
					exit.c \

SRC_BONUS =			pipex_bonus.c \
					here_doc_bonus.c\
					parent_bonus.c \
					child_bonus.c \
					path_bonus.c \
					init_bonus.c \
					clean_bonus.c \
					open_bonus.c \
					exit_bonus.c \

# **************************************************************************** #
#                              Progress Bars                                   #
# **************************************************************************** #
SRC_COUNT_TOT := $(shell echo -n $(SRCS) | wc -w)
ifeq ($(shell test $(SRC_COUNT_TOT) -le 0; echo $$?),0)
	SRC_COUNT_TOT := $(shell echo -n $(SRCS) | wc -w)
endif
SRC_COUNT := 0
SRC_PCT = $(shell expr 100 \* $(SRC_COUNT) / $(SRC_COUNT_TOT))

BONUS_COUNT_TOT := $(shell echo -n $(SRC_BONUS) | wc -w)
ifeq ($(shell test $(BONUS_COUNT_TOT) -le 0; echo $$?),0)
	BONUS_COUNT_TOT := $(shell echo -n $(SRC_BONUS) | wc -w)
endif
BONUS_COUNT := 0
BONUS_PCT = $(shell expr 100 \* $(BONUS_COUNT) / $(BONUS_COUNT_TOT))

# **************************************************************************** #
#                               Object File                                    #
# **************************************************************************** #
# Create directories
$(OBJ_DIR):
	@$(MKDIR) $(OBJ_DIR)

$(OBJ_BONUS_DIR):
	@$(MKDIR) $(OBJ_BONUS_DIR)

OBJS		= $(SRCS:%.c=$(OBJ_DIR)/%.o)
OBJS_BONUS 	= $(SRC_BONUS:%.c=$(OBJ_BONUS_DIR)/%.o)

# Rule to compile archive .c to ,o with progress bars
${OBJ_DIR}/%.o: ${SRC_DIR}/%.c $(DEPS) | $(OBJ_DIR)
	@$(eval SRC_COUNT = $(shell expr $(SRC_COUNT) + 1))
	@$(PRINT) "\r%100s\r[ %d/%d (%d%%) ] Compiling $(BLUE)$<$(DEFAULT)...\n" "" $(SRC_COUNT) $(SRC_COUNT_TOT) $(SRC_PCT)
	@$(CC) $(CFLAGS) -I. -c -o $@ $<

# Rule to compile archive .c to ,o with progress bars (Bonus)
$(OBJ_BONUS_DIR)/%.o: $(SRC_BONUS_DIR)/%.c $(DEPS) | $(OBJ_BONUS_DIR)
	@$(eval BONUS_COUNT = $(shell expr $(BONUS_COUNT) + 1))
	@$(PRINT) "\r%100s\r[ %d/%d (%d%%) ] Compiling $(MAGENTA)$<$(DEFAULT)...\n" "" $(BONUS_COUNT) $(BONUS_COUNT_TOT) $(BONUS_PCT)
	@$(CC) $(CFLAGS) -I. -c -o $@ $<

# **************************************************************************** #
#                              Targets                                         #
# **************************************************************************** #

all: $(LIBFT_A) $(NAME)

# Build pipex executable
$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) $(LIBFT_A) -I$(INC_DIR) -o $(NAME)
	@$(PRINT) "${CLEAR}${RESET}${GREY}────────────────────────────────────────────────────────────────────────────\n${RESET}${GREEN}»${RESET} [${PURPLE}${BOLD}${NAME}${RESET}]: ${RED}${BOLD}${NAME} ${RESET}compiled ${GREEN}successfully${RESET}.${GREY}\n${RESET}${GREY}────────────────────────────────────────────────────────────────────────────\n${RESET}"

# Build pipex_bonus executable
$(NAME_BONUS): $(OBJS_BONUS)
	@$(CC) $(CFLAGS) $(OBJS_BONUS) $(LIBFT_A) -I$(INC_DIR) -o $(NAME_BONUS)
	@$(PRINT) "${CLEAR}${RESET}${GREY}────────────────────────────────────────────────────────────────────────────\n${RESET}${GREEN}»${RESET} [${PURPLE}${BOLD}${NAME_BONUS}${RESET}]: ${RED}${BOLD}${NAME_BONUS} ${RESET}compiled ${GREEN}successfully${RESET}.${GREY}\n${RESET}${GREY}────────────────────────────────────────────────────────────────────────────\n${RESET}"

# Rebuild libft.a
$(LIBFT_A): $(LIBFT_MAKEFILE) $(LIBFT_H)
	@$(MAKE) -s -C $(LIBFT_DIR)

# Rule to compile bonus
bonus: $(LIBFT_A) $(NAME_BONUS)

# Test the norminette in my .c files
norm:
	@$(NORM) $(SRC_DIR)
	@$(NORM) $(INC_DIR)
	@$(NORM) $(LIBFT_DIR)

# Clean object files
clean:
	@$(MAKE) clean -s -C $(LIBFT_DIR)
	@$(RM) $(OBJ_DIR)
	@$(PRINT) "${CLEAR}${RESET}${GREEN}»${RESET} [${PURPLE}${BOLD}${NAME}${RESET}]: Objects were cleaned ${GREEN}successfully${RESET}.\n${RESET}"

# Full clean
fclean: clean
	@$(MAKE) fclean -s -C $(LIBFT_DIR)
	@$(RM) $(NAME)
	@$(RM) $(NAME_BONUS)
	@$(PRINT) "${CLEAR}${RESET}${GREY}────────────────────────────────────────────────────────────────────────────\n${RESET}${GREEN}»${RESET} [${PURPLE}${BOLD}${NAME}${RESET}]: Project cleaned ${GREEN}successfully${RESET}.${GREY}\n${RESET}${GREY}────────────────────────────────────────────────────────────────────────────\n${RESET}"

# Rebuild everything
re: fclean all

# Rebuild everything including bonus
rebonus: fclean bonus

# Phony targets
.PHONY: all bonus clean fclean re rebonus
.DEFAULT_GOAL := all
