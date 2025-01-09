# Program
NAME := RPN

# Necessities
CXX := c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

#Colors:
GREEN		=	\e[92;5;118m
YELLOW		=	\e[93;5;226m
GRAY		=	\e[33;2;37m
RESET		=	\e[0m
CURSIVE		=	\e[33;3m

# Targets
SRC := RPN.cpp main.cpp
INCLUDES := RPN.hpp 

# Rules
all: $(NAME)

$(NAME): $(SRC) $(INCLUDES)
	$(CXX) -o $@ $(CXXFLAGS) $(SRC)
	@printf "$(GREEN)Compilation successful!$(RESET)\n"

unit:
	$(MAKE) all CXXFLAGS="$(CXXFLAGS) -D _RPN_UNIT_TEST"

clean:
	rm -rf $(NAME)
	@printf "$(YELLOW)Executable removed.$(RESET)\n"
valgrind: | $(NAME)
	@printf "$(CURSIVE)Running valgrind...$(RESET)\n"
	valgrind --leak-check=full ./$(NAME)

re: clean all

.PHONY: all clean re
