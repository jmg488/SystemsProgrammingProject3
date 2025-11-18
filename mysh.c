#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/limits.h>

//Find command path by checking standard directories:
char* find_command_path(char* cmd) {
    char* search_paths[] = {"/usr/local/bin", "/usr/bin", "/bin", NULL};
    
    if (strchr(cmd, '/')) {
        if (access(cmd, X_OK) == 0) {
            return strdup(cmd);
        }
        return NULL;
    }
    
    for (int i = 0; search_paths[i] != NULL; i++) {
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", search_paths[i], cmd);
        if (access(full_path, X_OK) == 0) {
            return strdup(full_path);
        }
    }
    return NULL;
}

//Check if a command is a built-in:
int is_builtin(char* cmd){
	
    return (strcmp(cmd, "pwd") == 0 || 
            strcmp(cmd, "cd") == 0 || 
            strcmp(cmd, "which") == 0 ||
            strcmp(cmd, "echo") == 0 ||
            strcmp(cmd, "exit") == 0 ||
            strcmp(cmd, "die") == 0);
			
}

//Execute built-in commands, return exit status:
int execute_builtin(char** args){
	
    if(strcmp(args[0], "pwd") == 0){
		
        if(args[1] != NULL){
			
            fprintf(stderr, "pwd: too many arguments\n");
            return 1;
			
        }
		
        char buffer[PATH_MAX];
		
        if(getcwd(buffer, sizeof(buffer))){
			
            printf("%s\n", buffer);
            return 0;
			
        }
		
        perror("pwd");
        return 1;
		
    }
    
    if(strcmp(args[0], "cd") == 0){
		
        if(args[1] == NULL){
			
            fprintf(stderr, "cd: missing argument\n");
            return 1;
			
        }
		
        if(args[2] != NULL){
			
            fprintf(stderr, "cd: too many arguments\n");
            return 1;
			
        }
		
        if(chdir(args[1]) != 0){
			
            perror("cd");
            return 1;
			
        }
		
        return 0;
		
    }
    
    if(strcmp(args[0], "which") == 0){
		
        if(args[1] == NULL || args[2] != NULL){
			
            fprintf(stderr, "which: requires exactly one argument\n");
            return 1;
			
        }
        
        if(is_builtin(args[1])){
			
            return 1;
			
        }
        
        char* path = find_command_path(args[1]);
        
		if(path){
			
            printf("%s\n", path);
            free(path);
            return 0;
			
        }
		
        return 1;
		
    }
    
    if(strcmp(args[0], "echo") == 0){
		
        for(int i = 1; args[i] != NULL; i++){
			
            if (i > 1) printf(" ");
            printf("%s", args[i]);
			
        }
		
        printf("\n");
        return 0;
		
    }
    
    if(strcmp(args[0], "exit") == 0){
		
        if(args[1] != NULL){
			
            fprintf(stderr, "exit: too many arguments\n");
            return 1;
			
        }
		
        return 0;
		
    }
    
    if(strcmp(args[0], "die") == 0){
		
        for(int i = 1; args[i] != NULL; i++){
			
            if (i > 1) printf(" ");
            printf("%s", args[i]);
			
        }
		
        printf("\n");
        exit(EXIT_FAILURE);
		
    }
    
    return 1;
}

//Execute a single command (possibly built-in) with redirection:

int execute_single_command(char** args, char* input_file, char* output_file, int interactive, int from_stdin){
    
	if(args[0] == NULL){
		
        return 0;
		
    }
    
    //Handle the built-in commands:
    if(is_builtin(args[0])){
		
        //For exit, return the special code:
		
        if(strcmp(args[0], "exit") == 0){
			
            if(args[1] != NULL){
				
                fprintf(stderr, "exit: too many arguments\n");
                return 1;
				
            }
			
            return -999;  //Special code for exit.
			
        }
        
        //Fork for built-ins to handle redirection:
        pid_t pid = fork();
        
		if(pid < 0){
			
            perror("fork");
            return 1;
			
        }
        
        if(pid == 0){
			
            //Child process:
			
            if(output_file){
				
                int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0640);
				
                if(fd < 0){
					
                    perror(output_file);
                    exit(1);
					
                }
                
				if(dup2(fd, STDOUT_FILENO) < 0){
                    
					perror("dup2");
                    exit(1);
					
                }
				
                close(fd);
				
            }
            
            if(input_file){
				
                int fd = open(input_file, O_RDONLY);
                if(fd < 0){
					
                    perror(input_file);
                    exit(1);
					
                }
				
                if(dup2(fd, STDIN_FILENO) < 0){
					
                    perror("dup2");
                    exit(1);
					
                }
				
                close(fd);
				
            }
            
            //Redirect stdin to /dev/null in batch mode:
			
            if(!interactive && from_stdin && !input_file){
				
                int devnull = open("/dev/null", O_RDONLY);
                if(devnull >= 0){
					
                    dup2(devnull, STDIN_FILENO);
                    close(devnull);
					
                }
				
            }
            
            int status = execute_builtin(args);
            exit(status);
			
        }else{
			
            //Parent process:
			
            int status;
			
            waitpid(pid, &status, 0);
			
            if(WIFEXITED(status)){
				
                return WEXITSTATUS(status);
				
            }
			
            return 1;
			
        }
		
    }
    
    //Handle the external commands:
    pid_t pid = fork();
    
	if(pid < 0){
		
        perror("fork");
        return 1;
		
    }
    
    if(pid == 0){
		
        //Child process:
		
        if(output_file){
			
            int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0640);
			
            if(fd < 0){
				
                perror(output_file);
                exit(1);
				
            }
			
            if(dup2(fd, STDOUT_FILENO) < 0){
				
                perror("dup2");
                exit(1);
				
            }
			
            close(fd);
			
        }
        
        if(input_file){
			
            int fd = open(input_file, O_RDONLY);
			
            if(fd < 0){
				
                perror(input_file);
                exit(1);
				
            }
			
            if(dup2(fd, STDIN_FILENO) < 0){
				
                perror("dup2");
                exit(1);
				
            }
			
            close(fd);
			
        }
        
        //Redirect stdin to /dev/null in batch mode:
		
        if(!interactive && from_stdin && !input_file){
			
            int devnull = open("/dev/null", O_RDONLY);
            
			if(devnull >= 0){
				
                dup2(devnull, STDIN_FILENO);
                close(devnull);
				
            }
			
        }
        
        char* cmd_path = find_command_path(args[0]);
        
		if(cmd_path){
			
            execv(cmd_path, args);
            perror("execv");
            exit(1);
			
        }else{
			
            fprintf(stderr, "%s: command not found\n", args[0]);
            exit(1);
			
        }
		
    }else{
		
        //Parent process:
        int status;
        waitpid(pid, &status, 0);
		
        if(WIFEXITED(status)){
			
            return WEXITSTATUS(status);
			
        }
		
        return 1;
		
    }
	
}

//Execute a pipeline of n commands:
int execute_pipeline(char*** commands, int num_commands, int interactive, int from_stdin){
	
    if(num_commands == 1){
		
        return execute_single_command(commands[0], NULL, NULL, interactive, from_stdin);
		
    }
    
    int pipes[num_commands - 1][2];
    pid_t pids[num_commands];
    
    //Create all pipes:
    for(int i = 0; i < num_commands - 1; i++){
        
		if(pipe(pipes[i]) < 0){
			
            perror("pipe");
            return 1;
			
        }
		
    }
    
    //Fork all processes:
    for(int i = 0; i < num_commands; i++){
		
        pids[i] = fork();
		
        if(pids[i] < 0){
			
            perror("fork");
            return 1;
			
        }
        
        if(pids[i] == 0){
			
            //Child process:
            //Redirect stdin to /dev/null in batch mode for first command:
			
            if(i == 0 && !interactive && from_stdin){
				
                int devnull = open("/dev/null", O_RDONLY);
				
                if(devnull >= 0){
					
                    dup2(devnull, STDIN_FILENO);
                    close(devnull);
					
                }
				
            }
            
            //Connect to previous pipe (read end):
			
            if(i > 0){
				
                if(dup2(pipes[i - 1][0], STDIN_FILENO) < 0){
				
                    perror("dup2");
                    exit(1);
					
                }
				
            }
            
            //Connect to next pipe (write end):
			
            if(i < num_commands - 1){
				
                if(dup2(pipes[i][1], STDOUT_FILENO) < 0){
					
                    perror("dup2");
                    exit(1);
					
                }
				
            }
            
            //Close all pipe file descriptors:
            for(int j = 0; j < num_commands - 1; j++){
				
                close(pipes[j][0]);
                close(pipes[j][1]);
				
            }
            
            //Execute command:
            if(is_builtin(commands[i][0])){
				
                int status = execute_builtin(commands[i]);
                exit(status);
				
            }else{
				
                char* cmd_path = find_command_path(commands[i][0]);
				
                if(cmd_path){
					
                    execv(cmd_path, commands[i]);
                    perror("execv");
                    exit(1);
					
                }else{
					
                    fprintf(stderr, "%s: command not found\n", commands[i][0]);
                    exit(1);
					
                }
				
            }
			
        }
		
    }
    
    //Parent closes all pipes:
    for(int i = 0; i < num_commands - 1; i++){
		
        close(pipes[i][0]);
        close(pipes[i][1]);
		
    }
    
    //Wait for all children:
    int last_status = 0;
	
    for(int i = 0; i < num_commands; i++){
		
        int status;
        waitpid(pids[i], &status, 0);
		
        if(i == num_commands - 1){
			
            if(WIFEXITED(status)){
				
                last_status = WEXITSTATUS(status);
				
            }else{
				
                last_status = 1;
				
            }
			
        }
		
    }
    
    return last_status;
	
}
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
int main(int argc, char *argv[]){
	
    char input[PATH_MAX];
    int last_cmd_status = 0;
    int input_fd = STDIN_FILENO;
    int interactive = isatty(STDIN_FILENO);
    int from_stdin = 1; //If you're reading from stdin.

    //If a file argument is provided, open it:
    if(argc > 1){
		
        input_fd = open(argv[1], O_RDONLY);
		
        if(input_fd == -1){
			
            perror(argv[1]);
            exit(EXIT_FAILURE);
			
        }
		
        interactive = 0;
        from_stdin = 0;
		
    }

    if(interactive){
		
        printf("Welcome to my shell!\n");
		
    }

    while (1){
		
        if(interactive){
			
            printf("mysh> ");
            fflush(stdout);
			
        }
        
        //Read input from the correct file descriptor:
        ssize_t bytes_read = read(input_fd, input, sizeof(input) - 1);
		
        if(bytes_read <= 0){
			
            break;
			
        }
        
		input[bytes_read] = '\0';

        //Handles empty input:
        if(bytes_read == 1 && input[0] == '\n'){
			
            continue;
			
        }
        
        //Remove newline:
        input[strcspn(input, "\n")] = '\0';
        
        //Tokenize the input:
        char** tokens = malloc(PATH_MAX * sizeof(char*));
        int token_count = 0;
        char* token = strtok(input, " ");
        
        while(token != NULL && token_count < PATH_MAX - 1){
			
            tokens[token_count++] = token;
            token = strtok(NULL, " ");
			
        }
		
        tokens[token_count] = NULL;
        
        //Handle comments:
        for(int i = 0; i < token_count; i++){
			
            if(tokens[i][0] == '#'){
				
                tokens[i] = NULL;
                token_count = i;
                break;
				
            }
			
        }
        
        //Skip commands that are empty:
        if(token_count == 0 || tokens[0] == NULL){
			
            free(tokens);
            continue;
			
        }
        
        //Handle the conditionals:
        int skip_command = 0;
		
        if(strcmp(tokens[0], "and") == 0){
			
            if(last_cmd_status != 0){
				
                skip_command = 1;
				
            }
			
            //Shift tokens:
            for(int i = 0; i < token_count; i++){
				
                tokens[i] = tokens[i + 1];
				
            }
            
			token_count--;
			
        }else if(strcmp(tokens[0], "or") == 0){
			
            if(last_cmd_status == 0){
				
                skip_command = 1;
				
            }
			
            //Shift tokens:
            for(int i = 0; i < token_count; i++){
				
                tokens[i] = tokens[i + 1];
				
            }
			
            token_count--;
			
        }
		
		//////////////////////////////////////////////////////
		if(skip_command){
			
			free(tokens);
			continue;
			
		}

		if(token_count == 0 || tokens[0] == NULL){
			
			free(tokens);
			continue;
			
		}
		//////////////////////////////////////////////////////
		
        
		/*
        if(skip_command || token_count == 0 || tokens[0] == NULL){
			
            free(tokens);
            continue;
			
        }
        
		*/
        //Check for pipes:
        int pipe_count = 0;
        int pipe_positions[PATH_MAX];
        
        for(int i = 0; i < token_count; i++){
			
            if(strcmp(tokens[i], "|") == 0){
				
                pipe_positions[pipe_count++] = i;
				
            }
			
        }
        
        //Validate pipe syntax:
        if(pipe_count > 0){
			
            int valid = 1;
			
            for(int i = 0; i < pipe_count; i++){
				
                int pos = pipe_positions[i];
				
                if(pos == 0 || pos == token_count - 1 || 
                    strcmp(tokens[pos - 1], "|") == 0 || 
                    strcmp(tokens[pos + 1], "|") == 0){
						
                    fprintf(stderr, "syntax error: invalid pipe usage\n");
                    valid = 0;
                    last_cmd_status = 1;
                    break;
					
                }
				
            }
            
            if(!valid){
				
                free(tokens);
                continue;
				
            }
            
            //Build command array for pipeline:
            char*** commands = malloc((pipe_count + 1) * sizeof(char**));
            int cmd_idx = 0;
            int start = 0;
            
            for(int i = 0; i <= pipe_count; i++){
				
                int end = (i < pipe_count) ? pipe_positions[i] : token_count;
                int cmd_len = end - start;
                
                commands[cmd_idx] = malloc((cmd_len + 1) * sizeof(char*));
				
                for(int j = 0; j < cmd_len; j++){
					
                    commands[cmd_idx][j] = tokens[start + j];
					
                }
				
                commands[cmd_idx][cmd_len] = NULL;
                
                cmd_idx++;
                start = end + 1;
				
            }
            
            //Execute the pipeline:
            last_cmd_status = execute_pipeline(commands, pipe_count + 1, interactive, from_stdin);
            
            //Check for exit in pipeline:
            for(int i = 0; i <= pipe_count; i++){
				
                if(commands[i][0] && strcmp(commands[i][0], "exit") == 0){
					
                    if(commands[i][1] == NULL){
                        
						//Clean up:
                        
						for(int j = 0; j <= pipe_count; j++){
							
                            free(commands[j]);
							
                        }
						
                        free(commands);
                        free(tokens);
                        goto exit_shell;
						
                    }
					
                }
				
            }
            
            //Clean things up:
            for(int i = 0; i <= pipe_count; i++){
				
                free(commands[i]);
				
            }
			
            free(commands);
            free(tokens);
            continue;
			
        }
        
        //Handle redirection:
        char* input_file = NULL;
        char* output_file = NULL;
        char** command_args = malloc(PATH_MAX * sizeof(char*));
        int arg_count = 0;
        
        for(int i = 0; i < token_count; i++){
			
            if(strcmp(tokens[i], "<") == 0){
				
                if(i + 1 < token_count){
					
                    input_file = tokens[i + 1];
					
                    i++; //Skip the filename
					
                }else{
					
                    fprintf(stderr, "syntax error: expected filename after <\n");
                    last_cmd_status = 1;
                    free(command_args);
                    free(tokens);
                    goto next_iteration;
					
                }
				
            }else if(strcmp(tokens[i], ">") == 0){
				
                if(i + 1 < token_count){
					
                    output_file = tokens[i + 1];
                    i++;  //Skip the filename.
					
                }else{
					
                    fprintf(stderr, "syntax error: expected filename after >\n");
                    last_cmd_status = 1;
                    free(command_args);
                    free(tokens);
                    goto next_iteration;
					
                }
				
            }else{
				
                command_args[arg_count++] = tokens[i];
				
            }
			
        }
		
        command_args[arg_count] = NULL;
        
        //Execute the command:
		
        if(command_args[0]){
			
            int status = execute_single_command(command_args, input_file, output_file, interactive, from_stdin);
            
            //Check for the exit command:
			
            if(status == -999){
				
                free(command_args);
                free(tokens);
                break;
				
            }
            
            last_cmd_status = status;
			
        }
        
        free(command_args);
        free(tokens);
		
		next_iteration:
		continue;
		
    }

//Exit the shell:
exit_shell:
    if(input_fd != STDIN_FILENO){
		
        close(input_fd);
		
    }
    
    if(interactive){
		
        printf("mysh: exiting\n");
		
    }
    
    return EXIT_SUCCESS;
	
}//End mysh.c