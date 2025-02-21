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
    char *token = strtok(cmd_line, " ");
    while (token && cmd_buff->argc < CMD_ARGV_MAX - 1) {
        cmd_buff->argv[cmd_buff->argc++] = token;
        token = strtok(NULL, " ");
    }
    cmd_buff->argv[cmd_buff->argc] = NULL;
    return (cmd_buff->argc > 0) ? OK : WARN_NO_CMDS;
}

Built_In_Cmds identify_builtin_command(const char *command) {
    if (strcmp(command, "cd") == 0) return BI_CMD_CD;
    if (strcmp(command, "exit") == 0) return BI_CMD_EXIT;
    return BI_NOT_BI;
}

 int exec_local_cmd_loop()
{
    char input_buffer[SH_CMD_MAX];
    command_buffer_t cmd_buffer;

    // TODO IMPLEMENT MAIN LOOP

    // TODO IMPLEMENT parsing input to cmd_buff_t *cmd_buff

    // TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
    // the cd command should chdir to the provided directory; if no directory is provided, do nothing

    // TODO IMPLEMENT if not built-in command, fork/exec as an external command
    // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"

    while (1) {
        printf("%s", SH_PROMPT);
        fflush(stdout);

        if (fgets(input_buffer, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }

        input_buffer[strcspn(input_buffer, "\n")] = '\0';

        if (strcmp(input_buffer, EXIT_CMD) == 0) {
            exit(OK);
        }

        memset(&cmd_buffer, 0, sizeof(cmd_buffer));
        if (parse_command(input_buffer, &cmd_buffer) != OK) {
            printf("%s\n", CMD_WARN_NO_CMD);
            continue;
        }

        Built_In_Cmds builtin_cmd = identify_builtin_command(cmd_buffer.arguments[0]);
        if (builtin_cmd == BI_CMD_CD) {
            if (cmd_buffer.arg_count > 1) {
                if (chdir(cmd_buffer.arguments[1]) != 0) {
                    perror("cd");
                }
            }
            continue;
        } else if (builtin_cmd == BI_CMD_EXIT) {
            exit(OK);
        }

        int exit_status = execute_command(&cmd_buffer);
        if (exit_status != 0) {
            exit(exit_status);
        }
    }

    return OK;
}

int execute_command(command_buffer_t *cmd_buffer) {
    pid_t process_id = fork();
    if (process_id < 0) {
        perror("fork failed");
        return ERR_EXEC_CMD;
    } else if (process_id == 0) {
        execvp(cmd_buffer->arguments[0], cmd_buffer->arguments);
        perror("execvp failed");
        exit(250);
    } else { 
        int status;
        waitpid(process_id, &status, 0);
        return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    }
}