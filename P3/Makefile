CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -g
TARGET = mysh
SRC = mysh.c

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
	rm -f test_*.txt
	rm -f pipe_redirect_output.txt
	rm -rf test_dir

test: $(TARGET)
	@bash run_tests.sh

# Run individual test categories
test-builtins: $(TARGET)
	@echo "Running built-in command tests..."
	@./mysh tests/test_pwd.txt
	@./mysh tests/test_echo.txt
	@./mysh tests/test_which_ls.txt

test-pipes: $(TARGET)
	@echo "Running pipe tests..."
	@./mysh tests/test_pipe_simple.txt
	@./mysh tests/test_pipe_multiple.txt

test-redirects: $(TARGET)
	@echo "Running redirection tests..."
	@./mysh tests/test_redirect_output.txt

test-conditionals: $(TARGET)
	@echo "Running conditional tests..."
	@./mysh tests/test_and_success.txt
	@./mysh tests/test_or_failure.txt