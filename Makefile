NAME		:=	midona

COMPILER	:=	clang++
FLAGS		:=	-Wall -Wextra -c -I. -std=c++11 -march=native

ifdef DEBUG
FLAGS		+= -g3 -DDEBUG
else
FLAGS		+= -O3
endif

OBJECT_DIR	= .objects

SOURCES		= $(wildcard *.cpp */*.cpp */*/*.cpp */*/*/*.cpp)
OBJECTS		= $(patsubst %.cpp,$(OBJECT_DIR)/%.o,$(SOURCES))

$(OBJECT_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@printf "\x1b[0;32m[+] Compiling %s\x1b[0m\n" $@
	@$(COMPILER) $(FLAGS) $< -o $@

all: $(NAME)

$(NAME): $(OBJECTS)
	@printf "\x1b[0;32m[+] Linking %s\x1b[0m\n" $@
	@$(COMPILER) $^ -o $@

clean:
	@printf "\x1b[0;31m[+] Removing %s\x1b[0m\n" $(OBJECTS)
	@rm -f $(OBJECTS)
	@printf "\x1b[0;31m[+] Removing %s\x1b[0m\n" $(OBJECT_DIR)
	@rm -rf $(OBJECT_DIR)

fclean: clean
	@printf "\x1b[0;31m[+] Removing %s\x1b[0m\n" $(NAME)
	@rm -f $(NAME)

re: fclean $(NAME)

test_cli: re
	@printf "===== CLI TEST =====\n"
	@printf "TEST 01 : NO PARAMETER GIVEN\n"
	./$(NAME)
	@printf "\nTEST 02 : INPUT FILE ONLY GIVEN\n"
	./$(NAME) $(NAME)
	@printf "\nTEST 03 : INPUT FILE WITH ACTION\n"
	./$(NAME) -b $(NAME)
	@printf "\nTEST 04 : ANY ORDER 1\n"
	./$(NAME) ./$(NAME) -c
	@printf "\nTEST 05 : ANY ORDER 2\n"
	./$(NAME) -e -o output $(NAME)
	@printf "\nTEST 05 : HELP PARAM\n"
	./$(NAME) --help
	@printf "\nTEST 06 : VERSION PARAM\n"
	./$(NAME) -v
	@printf "\nTEST 07 : HELP PARAM WITH INPUT FILE\n"
	./$(NAME) --help ./$(NAME)
	@printf "\nTEST 08 : VERSION PARAM WITH INPUT FILE\n"
	./$(NAME) -v ./$(NAME)

.PHONY : re fclean clean $(NAME) test
