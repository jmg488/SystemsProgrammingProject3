===================================================================
This is the README for the MyShell (mysh) project:
===================================================================
Authors:

Jeffrey Green
NETID: jmg488

Jose Sosa
NETID: js4069
===================================================================
- Jeff will be submitting the .tar archive to Canvas for submission.
- To build/compile the project, type "make" (without quotations) in bash.
===================================================================
FIRST TIME SETUP (PLEASE READ!!!!!):
===================================================================
Step #1 (Only need to do it one time): Create the test and input files: bash create_test_files.sh

Step#2 (Only need to do it one time): Make the test runner executable: chmod +x run_tests.sh

Step #3: To only compile mysh and nothing else: make
Step #3: To compile AND run all of the automated test: make test

Step #4: To clean up: make clean
===================================================================
FOR RUNNING THE SHELL:
===================================================================
Option #1: Interactive Mode:
./mysh

Option #2: Batch Mode (from a file):
./mysh script.sh

Option #3: Batch Mode (stdin):
cat script.sh | ./mysh
===================================================================
FOR RUNNING THE TESTS:
===================================================================
Create test files (you only do this the first time):
bash create_test_files.sh

Run all tests:
make tests

To run specific categories of tests:
make test-builtins
make test-pipes
make test-redirects
make test-conditionals
===================================================================
Features:
===================================================================
- The built-ins:
	- pwd
	- cd
	- which
	- echo
	- exit
	- die
	
- Redirection:
	- Input redirection.
	- Output redirection.
	- Input AND output redirection.
	
- Pipelines: 
	- One, single pipe.
	- Two, or more, pipes.
	- Additional imlpementation for "n" number of pipes.
	
- Conditionals
	- Use of "and," where you execute only if the previous command succeeded.
	- Use of "or," where you only execute if the previous command failed.
	
- Additional features:
	- Absolute paths.
	- Relative paths.
	- External command execution with path search.
	- Error handling and reporting.
	
===================================================================
Test Plan:
===================================================================
Test Strategy:

- Automated test runner with run_test.sh.
	- This executes all test cases and reports the results.
	
- Individual test files (e.g., text/*.txt).
	- To test isolated scenarios.
	
- Output verification:
	- Where you check both stdout and exit codes.
	
- Manual testing for the interactive mode.
===================================================================
Testing Coverage:

- Five test cases for built-in commands:

	- test_pwd.txt
	- test_cd.txt
	- test_echo.txt
	- test_which_ls.txt
	- test_which_builtin.txt
	
	Summary of what we're validating:
	Making sure we have the correct output format.
	Making sure that we have the proper error message being
	thrown for any arguments that are invalid.
	Validating exit status 0/1.

- Three test cases for redirection:

	- test_redirect_output.txt
	- test_redirect_input.txt
	- test_redirect_both.txt
	
	Summary of what we're validating:
	Making sure that output files are being created correctly. 
	Also validating file permissions.
	Making sure that, for missing files, the error handling is 
	being done properly.
	

- Three test cases for piping:

	- test_pipe_simple.txt
	- test_pipe_multiple.txt
	- test_pipe_builtin.txt
	
	Summary of what we're validating:
	Making sure that the data that is going through the pipeline properly.
	Making sure that the exit status is used for the last command.
	Making sure that any/all processes that were created are getting cleaned up correctly.
	
- Four test cases for the conditionals:

	- test_and_success.txt
	- test_and_failure.txt
	- test_or_success.txt
	- test_or_failure.txt
	
	Summary of what we're validating:
	Making sure that commands only run when a conditional is met.
	Making sure that we're properly tracking exit status.
	Making sure that the conditional applies to the ENTIRE command.
	We're doing two tests for AND and two for OR.
	
- Five test cases for some edge cases:
	- test_comments.txt
	- test_empty_lines.txt
	- test_not_found.txt
	- test_exit.txt
	- test_die.txt
	
	Summary of what we're validating:
	Making sure that comments are ignored.
	Making sure any empty lines aren't going to cause errors.
	Making sure that error messages are appropriate.
	Also checking for correct exit codes.
	
- Three test cases for external commands:

	- test_ls.txt
	- test_cat.txt
	- test_absolute_path.txt
	
	Summary of what we're validating:
	Making sure that commands that weren't found and
	give an error are properly handled. Checking that
	absolute paths are working properly. 
	
- Three tests for more complex scenarios:

	- test_complex_conditionals.txt
	- test_pipe_with_redirect.txt
	- test_multiple_operations.txt
	
	Summary of what we're validating:
	We're seeing if the program works properly
	when performing a bunch of conditionals,
	combining pipelines with redirects, and 
	also testing several operations sequentially.
	These are all real-world scenarios.
===================================================================
Doing testing in batch mode:
===================================================================
- Note that all of the automated tests are run in batch mode.
- We are verifying that no prompts/welcome messages are being printed.
- We are also confirming that std is redirected to /dev/null for child processes.
===================================================================
Doing testing in interactive mode (i.e., testing manually):
===================================================================
- You would run ./mysh directly.
- We are verifying the following:
	- That the welcome message is printed.
	- That the "mysh>" is shown.
	- That we get a goodbye message printed upon exit.
	- And to test that commands also execute interactively.
===================================================================
Included Files:
===================================================================
mysh.c (Source code in C for this project.  Just one C file this time around.)
Makefile
README.txt
run-tests.sh
create_test_files.sh
tests/*.txt
===================================================================
Cleaning Up:
===================================================================
make clean

This will remove the executable and remove the temporary
test files.
===================================================================