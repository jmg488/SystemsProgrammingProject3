# Create all test files at once with this script
# Run: bash create_test_files.sh

mkdir -p tests

# ================================
# BUILT-IN COMMAND TESTS
# ================================

# test_pwd.txt
cat > tests/test_pwd.txt << 'EOF'
pwd
exit
EOF

# test_cd.txt
cat > tests/test_cd.txt << 'EOF'
cd tests
pwd
exit
EOF

# test_echo.txt
cat > tests/test_echo.txt << 'EOF'
echo hello world
exit
EOF

# test_which_ls.txt
cat > tests/test_which_ls.txt << 'EOF'
which ls
exit
EOF

# test_which_builtin.txt
cat > tests/test_which_builtin.txt << 'EOF'
which cd
exit
EOF

# ================================
# REDIRECTION TESTS
# ================================

# test_redirect_output.txt
cat > tests/test_redirect_output.txt << 'EOF'
echo redirected output > test_output.txt
exit
EOF

# test_redirect_input.txt
cat > tests/test_redirect_input.txt << 'EOF'
cat < test_input.txt > test_input_output.txt
exit
EOF

# test_redirect_both.txt
cat > tests/test_redirect_both.txt << 'EOF'
cat < test_both_in.txt > test_both_out.txt
exit
EOF

# ================================
# PIPE TESTS
# ================================

# test_pipe_simple.txt
cat > tests/test_pipe_simple.txt << 'EOF'
ls | grep mysh
exit
EOF

# test_pipe_multiple.txt
cat > tests/test_pipe_multiple.txt << 'EOF'
echo pipe test | cat | cat | cat
exit
EOF

# test_pipe_builtin.txt
cat > tests/test_pipe_builtin.txt << 'EOF'
pwd | cat
exit
EOF

# ================================
# CONDITIONAL TESTS
# ================================

# test_and_success.txt
cat > tests/test_and_success.txt << 'EOF'
echo first command
and echo second command
exit
EOF

# test_and_failure.txt
cat > tests/test_and_failure.txt << 'EOF'
nonexistent_command_xyz
and echo should not print
exit
EOF

# test_or_success.txt
cat > tests/test_or_success.txt << 'EOF'
echo first works
or echo should not print
exit
EOF

# test_or_failure.txt
cat > tests/test_or_failure.txt << 'EOF'
nonexistent_command_abc
or echo this runs
exit
EOF

# ================================
# EDGE CASE TESTS
# ================================

# test_comments.txt
cat > tests/test_comments.txt << 'EOF'
echo visible # this is a comment
# this entire line is a comment
echo also visible
exit
EOF

# test_empty_lines.txt
cat > tests/test_empty_lines.txt << 'EOF'


echo after empty
exit
EOF

# test_not_found.txt
cat > tests/test_not_found.txt << 'EOF'
this_command_does_not_exist_anywhere
exit
EOF

# test_exit.txt
cat > tests/test_exit.txt << 'EOF'
echo testing exit
exit
EOF

# test_die.txt
cat > tests/test_die.txt << 'EOF'
die error message for die command
EOF

# ================================
# EXTERNAL COMMAND TESTS
# ================================

# test_ls.txt
cat > tests/test_ls.txt << 'EOF'
ls
exit
EOF

# test_cat.txt
cat > tests/test_cat.txt << 'EOF'
cat test_cat_file.txt
exit
EOF

# test_absolute_path.txt
cat > tests/test_absolute_path.txt << 'EOF'
/bin/echo absolute path test
exit
EOF

# ================================
# COMPLEX SCENARIO TESTS
# ================================

# test_complex_conditionals.txt
cat > tests/test_complex_conditionals.txt << 'EOF'
echo first
and echo second
and echo third
exit
EOF

# test_pipe_with_redirect.txt
cat > tests/test_pipe_with_redirect.txt << 'EOF'
echo hello world | cat > pipe_redirect_output.txt
exit
EOF

# test_multiple_operations.txt
cat > tests/test_multiple_operations.txt << 'EOF'
pwd
cd /tmp
pwd
cd -
echo done
exit
EOF

echo "All test files created in tests/ directory!"