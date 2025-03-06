#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    if (!cmd_line || !cmd_buff) return ERR_MEMORY;

    cmd_buff->argc = 0;
    char *tok = strtok(cmd_line, " ");
    
    while (tok && cmd_buff->argc < CMD_ARGV_MAX - 1) {
        cmd_buff->argv[cmd_buff->argc++] = tok;
        tok = strtok(NULL, " ");
    }
    
    cmd_buff->argv[cmd_buff->argc] = NULL; // Null-terminate the argument list
    return (cmd_buff->argc > 0) ? OK : WARN_NO_CMDS;
}

 /*
 * Parses a command line containing pipes into a command_list_t structure,
 * handling multiple piped commands.
 */
int build_cmd_list(char *cmd_line, command_list_t *clist) {
    if (!cmd_line || !clist) return ERR_MEMORY;

    clist->num = 0;  
    char *location;
    char *tok = strtok_r(cmd_line, PIPE_STRING, &location);

    while (tok && clist->num < CMD_MAX) {
        while (*tok == ' ') tok++;  // Trim leading spaces

        char *end = tok + strlen(tok) - 1;
        while (end > tok && *end == ' ') end--;  // Trim trailing spaces
        *(end + 1) = '\0';

        if (strlen(tok) == 0) {  // Skip empty commands
            tok = strtok_r(NULL, PIPE_STRING, &location);
            continue;
        }

        if (build_cmd_buff(tok, &clist->commands[clist->num]) != OK) {
            return ERR_CMD_ARGS_BAD;
        }

        clist->num++;
        tok = strtok_r(NULL, PIPE_STRING, &location);
    }

    return (clist->num > 0) ? OK : WARN_NO_CMDS;
}

/*
 * Executes a pipeline of commands by creating child processes and using pipes.
 */
int execute_pipeline(command_list_t *clist) {
    int num_commands = clist->num;
    int pipefds[2 * (num_commands - 1)]; // Array to store pipe file descriptors
    pid_t pids[num_commands];

    // Create pipes for inter-process communication
    for (int i = 0; i < num_commands - 1; i++) {
        if (pipe(pipefds + 2 * i) < 0) {
            perror("pipe failed");
            return ERR_EXEC_CMD;
        }
    }

    // Fork child processes for each command
    for (int i = 0; i < num_commands; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork failed");
            return ERR_EXEC_CMD;
        }

        if (pids[i] == 0) { // Child process
            if (i > 0) {
                dup2(pipefds[2 * (i - 1)], STDIN_FILENO); // Read from previous pipe
            }
            
            if (i < num_commands - 1) {
                dup2(pipefds[2 * i + 1], STDOUT_FILENO); // Write to next pipe
            }
            
            for (int j = 0; j < 2 * (num_commands - 1); j++) {
                close(pipefds[j]); // Close all pipes in the child process
            }
            
            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror("execvp failed");
            exit(250);
        }
    }

    // Close all pipes in the parent process
    for (int i = 0; i < 2 * (num_commands - 1); i++) {
        close(pipefds[i]);
    }

    // Wait for all child processes to finish
    for (int i = 0; i < num_commands; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            return WEXITSTATUS(status);
        }
    }

    return OK;
}

/*
 * Executes a single command without pipes.
 */
int exec_cmd(cmd_buff_t *cmd) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return ERR_EXEC_CMD;
    } else if (pid == 0) { // Child process
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp failed");
        exit(250);
    } else { // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else {
            return -1;
        }
    }
}

/*
 * Main loop for the shell program, continuously prompting for user input
 * and executing commands.
 */
int exec_local_cmd_loop() {
    char cmd_buff[SH_CMD_MAX];
    command_list_t clist;

    while (1) {
        printf("%s", SH_PROMPT);
        fflush(stdout);

        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL) {
            printf("\n"); // Handle EOF
            break;
        }

        cmd_buff[strcspn(cmd_buff, "\n")] = '\0'; // Remove newline character

        if (strcmp(cmd_buff, EXIT_CMD) == 0) { // Check for exit command
            break;
        }
        
        memset(&clist, 0, sizeof(clist)); // Initialize command list structure
        
        if (build_cmd_list(cmd_buff, &clist) != OK) {
            printf("%s\n", CMD_WARN_NO_CMD);
            continue;
        }
        
        // Execute commands based on whether pipes are used
        if (clist.num > 1) {
            execute_pipeline(&clist);
        } else {
            exec_cmd(&clist.commands[0]);
        }
    }

    return OK;
}
