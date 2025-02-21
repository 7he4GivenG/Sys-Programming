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

 int parse_input_line(char *input_line, cmd_buff_t *cmd_struct) {
    if (!input_line || !cmd_struct) return ERR_MEMORY;
    
    cmd_struct->argc = 0;
    char *token = strtok(input_line, " ");
    while (token && cmd_struct->argc < CMD_ARGV_MAX - 1) {
        cmd_struct->argv[cmd_struct->argc++] = token;
        token = strtok(NULL, " ");
    }
    cmd_struct->argv[cmd_struct->argc] = NULL;
    return (cmd_struct->argc > 0) ? OK : WARN_NO_CMDS;
}

Built_In_Cmds identify_builtin_cmd(const char *command) {
    if (strcmp(command, "cd") == 0) return BI_CMD_CD;
    if (strcmp(command, "exit") == 0) return BI_CMD_EXIT;
    return BI_NOT_BI;
}

int execute_external_cmd(cmd_buff_t *cmd_struct) {
    pid_t process_id = fork();
    if (process_id < 0) {
        perror("fork failed");
        return ERR_EXEC_CMD;
    } else if (process_id == 0) {
        execvp(cmd_struct->argv[0], cmd_struct->argv);
        perror("execvp failed");
        exit(250);
    } else {
        int process_status;
        waitpid(process_id, &process_status, 0);
        if (WIFEXITED(process_status)) {
            return WEXITSTATUS(process_status);
        } else {
            return -1;
        }
    }
}

 int exec_local_cmd_loop()
{
    char input_buffer[SH_CMD_MAX];
    cmd_buff_t parsed_command;

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

        memset(&parsed_command, 0, sizeof(parsed_command));
        if (parse_input_line(input_buffer, &parsed_command) != OK) {
            printf("%s\n", CMD_WARN_NO_CMD);
            continue;
        }

        Built_In_Cmds builtin_cmd = identify_builtin_cmd(parsed_command.argv[0]);
        if (builtin_cmd == BI_CMD_CD) {
            if (parsed_command.argc > 1) {
                if (chdir(parsed_command.argv[1]) != 0) {
                    perror("cd");
                }
            }
            continue;
        } else if (builtin_cmd == BI_CMD_EXIT) {
            exit(OK);
        }

        int execution_status = execute_external_cmd(&parsed_command);

        if (execution_status != 0) {
            exit(execution_status);
        }
    }

    return OK;
}


