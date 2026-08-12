NAME		:=	midona

COMPILER	:=	g++
FLAGS		:=	-Wall -Wextra -c -I.

ifdef DEBUG
FLAGS		+= -g3
else
FLAGS		+= -O3
endif

OBJECT_DIR	= .objects

SOURCES		= $(wildcard *.cpp */*.cpp */*/*.cpp)
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

fclean: clean
	@printf "\x1b[0;31m[+] Removing %s\x1b[0m\n" $(NAME)
	@rm -f $(NAME)

re: fclean $(NAME)

.PHONY : re fclean clean $(NAME)
