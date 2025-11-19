#!/bin/bash

# Color codes for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

PASSED=0
FAILED=0

# Compile mysh
echo "Compiling mysh..."
make clean > /dev/null 2>&1
make > /dev/null 2>&1

if [ ! -f "./mysh" ]; then
    echo -e "${RED}Error: mysh failed to compile${NC}"
    exit 1
fi

echo -e "${GREEN}mysh compiled successfully${NC}\n"

# Function to run a test
run_test() {
    local test_name=$1
    local input_file=$2
    local expected_output=$3
    local check_type=$4  # "exact" or "contains"
    
    echo -n "Testing $test_name... "
    
    if [ ! -f "$input_file" ]; then
        echo -e "${RED}FAIL${NC} (input file not found)"
        ((FAILED++))
        return
    fi
    
    actual_output=$(./mysh "$input_file" 2>&1)
    
    if [ "$check_type" == "exact" ]; then
        if [ "$actual_output" == "$expected_output" ]; then
            echo -e "${GREEN}PASS${NC}"
            ((PASSED++))
        else
            echo -e "${RED}FAIL${NC}"
            echo "  Expected: $expected_output"
            echo "  Got:      $actual_output"
            ((FAILED++))
        fi
    elif [ "$check_type" == "contains" ]; then
        if echo "$actual_output" | grep -q "$expected_output"; then
            echo -e "${GREEN}PASS${NC}"
            ((PASSED++))
        else
            echo -e "${RED}FAIL${NC}"
            echo "  Expected to contain: $expected_output"
            echo "  Got: $actual_output"
            ((FAILED++))
        fi
    fi
}

# Function to run a test and check exit code
run_test_exit_code() {
    local test_name=$1
    local input_file=$2
    local expected_exit=$3
    
    echo -n "Testing $test_name (exit code)... "
    
    ./mysh "$input_file" > /dev/null 2>&1
    actual_exit=$?
    
    if [ $actual_exit -eq $expected_exit ]; then
        echo -e "${GREEN}PASS${NC}"
        ((PASSED++))
    else
        echo -e "${RED}FAIL${NC}"
        echo "  Expected exit code: $expected_exit"
        echo "  Got exit code:      $actual_exit"
        ((FAILED++))
    fi
}

echo "================================"
echo "BUILT-IN COMMAND TESTS"
echo "================================"

# Test pwd
run_test "pwd" "tests/test_pwd.txt" "$(pwd)" "contains"

# Test cd and pwd
mkdir -p test_dir
run_test "cd" "tests/test_cd.txt" "test_dir" "contains"
rmdir test_dir

# Test echo
run_test "echo" "tests/test_echo.txt" "hello world" "contains"

# Test which
run_test "which ls" "tests/test_which_ls.txt" "/bin/ls" "contains"

# Test which with built-in (should fail/no output for built-in)
echo -n "Testing which with built-in (should fail)... "
output=$(./mysh tests/test_which_builtin.txt 2>&1)
if [ -z "$output" ] || echo "$output" | grep -qv "/"; then
    echo -e "${GREEN}PASS${NC}"
    ((PASSED++))
else
    echo -e "${RED}FAIL${NC}"
    echo "  which should not find built-ins"
    ((FAILED++))
fi

echo ""
echo "================================"
echo "REDIRECTION TESTS"
echo "================================"

# Test output redirection
./mysh tests/test_redirect_output.txt > /dev/null 2>&1
if [ -f "test_output.txt" ]; then
    content=$(cat test_output.txt)
    if [ "$content" == "redirected output" ]; then
        echo -e "Testing output redirection... ${GREEN}PASS${NC}"
        ((PASSED++))
    else
        echo -e "Testing output redirection... ${RED}FAIL${NC}"
        ((FAILED++))
    fi
    rm -f test_output.txt
else
    echo -e "Testing output redirection... ${RED}FAIL${NC}"
    ((FAILED++))
fi

# Test input redirection
echo "test input data" > test_input.txt
./mysh tests/test_redirect_input.txt > /dev/null 2>&1
if [ -f "test_input_output.txt" ]; then
    content=$(cat test_input_output.txt)
    if echo "$content" | grep -q "test input data"; then
        echo -e "Testing input redirection... ${GREEN}PASS${NC}"
        ((PASSED++))
    else
        echo -e "Testing input redirection... ${RED}FAIL${NC}"
        ((FAILED++))
    fi
    rm -f test_input.txt test_input_output.txt
else
    echo -e "Testing input redirection... ${RED}FAIL${NC}"
    ((FAILED++))
fi
rm -f test_input.txt

# Test both input and output redirection
echo "both redirect test" > test_both_in.txt
./mysh tests/test_redirect_both.txt > /dev/null 2>&1
if [ -f "test_both_out.txt" ]; then
    content=$(cat test_both_out.txt)
    if echo "$content" | grep -q "both redirect test"; then
        echo -e "Testing input+output redirection... ${GREEN}PASS${NC}"
        ((PASSED++))
    else
        echo -e "Testing input+output redirection... ${RED}FAIL${NC}"
        ((FAILED++))
    fi
    rm -f test_both_in.txt test_both_out.txt
else
    echo -e "Testing input+output redirection... ${RED}FAIL${NC}"
    ((FAILED++))
fi
rm -f test_both_in.txt

echo ""
echo "================================"
echo "PIPE TESTS"
echo "================================"

# Test simple pipe
run_test "simple pipe (ls | grep mysh)" "tests/test_pipe_simple.txt" "mysh" "contains"

# Test multiple pipes
run_test "multiple pipes (echo | cat | cat | cat)" "tests/test_pipe_multiple.txt" "pipe test" "contains"

# Test pipe with built-in
run_test "pipe with built-in (pwd | cat)" "tests/test_pipe_builtin.txt" "$(pwd)" "contains"

echo ""
echo "================================"
echo "CONDITIONAL TESTS"
echo "================================"

# Test 'and' success
run_test "'and' with success" "tests/test_and_success.txt" "second command" "contains"

# Test 'and' failure
echo -n "Testing 'and' with failure (should not run second)... "
output=$(./mysh tests/test_and_failure.txt 2>&1)
if echo "$output" | grep -qv "should not print"; then
    echo -e "${GREEN}PASS${NC}"
    ((PASSED++))
else
    echo -e "${RED}FAIL${NC}"
    ((FAILED++))
fi

# Test 'or' success
echo -n "Testing 'or' with success (should not run second)... "
output=$(./mysh tests/test_or_success.txt 2>&1)
if echo "$output" | grep -qv "should not print"; then
    echo -e "${GREEN}PASS${NC}"
    ((PASSED++))
else
    echo -e "${RED}FAIL${NC}"
    ((FAILED++))
fi

# Test 'or' failure
run_test "'or' with failure" "tests/test_or_failure.txt" "this runs" "contains"

echo ""
echo "================================"
echo "EDGE CASE TESTS"
echo "================================"

# Test comments
run_test "comments" "tests/test_comments.txt" "visible" "contains"

# Test empty lines
echo -n "Testing empty lines... "
output=$(./mysh tests/test_empty_lines.txt 2>&1)
if echo "$output" | grep -q "after empty"; then
    echo -e "${GREEN}PASS${NC}"
    ((PASSED++))
else
    echo -e "${RED}FAIL${NC}"
    ((FAILED++))
fi

# Test command not found
echo -n "Testing command not found... "
output=$(./mysh tests/test_not_found.txt 2>&1)
if echo "$output" | grep -q "not found\|No such file"; then
    echo -e "${GREEN}PASS${NC}"
    ((PASSED++))
else
    echo -e "${RED}FAIL${NC}"
    ((FAILED++))
fi

# Test exit command
run_test_exit_code "exit command" "tests/test_exit.txt" 0

# Test die command (should exit with failure)
run_test_exit_code "die command" "tests/test_die.txt" 1

echo ""
echo "================================"
echo "EXTERNAL COMMAND TESTS"
echo "================================"

# Test ls
run_test "ls command" "tests/test_ls.txt" "mysh" "contains"

# Test cat
echo "cat test content" > test_cat_file.txt
run_test "cat command" "tests/test_cat.txt" "cat test content" "contains"
rm -f test_cat_file.txt

# Test absolute path
run_test "absolute path (/bin/echo)" "tests/test_absolute_path.txt" "absolute path test" "contains"

echo ""
echo "================================"
echo "INTERACTIVE MODE TEST"
echo "================================"

# Test interactive mode (manual check)
echo -e "${YELLOW}Note: Interactive mode should be tested manually${NC}"
echo -e "${YELLOW}Run: ./mysh${NC}"
echo -e "${YELLOW}Expected: Welcome message and 'mysh>' prompt${NC}"

echo ""
echo "================================"
echo "TEST SUMMARY"
echo "================================"
echo -e "Passed: ${GREEN}$PASSED${NC}"
echo -e "Failed: ${RED}$FAILED${NC}"
echo "================================"

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed.${NC}"
    exit 1
fi