CC = gcc
CFLAGS = -Wall -Wno-unused-function -std=c99 -g
LDFLAGS =
SNAKE_DEPS = snake.o snake_utils.o state.o
INTERACTIVE_DEPS = interactive_snake.o snake_utils.o state.o
UNIT_TESTS_DEPS = snake_utils.o unit_tests.o
TESTS = 1-simple 2-direction 3-tail 4-food 5-wall 6-small 7-large 8-multisnake 9-everything

COLOR_GREEN =
COLOR_RESET =
ifeq (,${NO_COLOR})
	ifneq (,$(findstring xterm,${TERM}))
		override COLOR_GREEN := $(shell tput setaf 2)
		override COLOR_RESET := $(shell tput sgr0)
	endif
endif

help:
	@echo Usage:
	@echo make run-unit-tests: Compiles and runs unit tests.
	@echo make debug-unit-tests: Compiles unit tests and starts the debugger.
	@echo make valgrind-unit-tests: Compiles unit tests and runs them under Valgrind.
	@echo make run-integration-tests: Compiles and runs integration tests.
	@echo make snake: Compiles the snake executable.
	@echo make interactive-snake: Compiles the interactive snake executable.
	@echo make clean: Removes executables and output files.

.PHONY: all
all: interactive-snake snake unit-tests

snake: $(SNAKE_DEPS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

interactive-snake: $(INTERACTIVE_DEPS)
	$(CC) -o $@ $^ -pthread $(CFLAGS) $(LDFLAGS)

unit-tests: $(UNIT_TESTS_DEPS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

unit_tests.o: unit_tests.c state.c
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f interactive-snake snake unit-tests unit-test-*.snk *.exe *.o

.PHONY: debug-unit-tests
debug-unit-tests: unit-tests
	cgdb ./unit-tests

.PHONY: run-unit-tests
run-unit-tests: unit-tests
	./unit-tests

.PHONY: valgrind-unit-tests
valgrind-unit-tests: unit-tests
	valgrind --leak-check=full --track-origins=yes ./unit-tests -m

.PHONY: run-integration-tests
run-integration-tests: $(TESTS)

.PHONY: $(TESTS)
$(TESTS): snake
	@sed 's/\r$$//g' "tests/$(@F)-in.snk" > "tests/.$(@F)-in.snk.tmp"
	@mv "tests/.$(@F)-in.snk.tmp" "tests/$(@F)-in.snk"
	@sed 's/\r$$//g' "tests/$(@F)-ref.snk" > "tests/.$(@F)-ref.snk.tmp"
	@mv "tests/.$(@F)-ref.snk.tmp" "tests/$(@F)-ref.snk"
	./snake -i "tests/$(@F)-in.snk" -o "tests/$(@F)-out.snk"
	diff "tests/$(@F)-ref.snk" "tests/$(@F)-out.snk"
	@echo "${COLOR_GREEN}Passed $(@F)${COLOR_RESET}"
