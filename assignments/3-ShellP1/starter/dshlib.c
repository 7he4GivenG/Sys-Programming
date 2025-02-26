#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist) {
    // Ensure cmd_line is not NULL or empty before processing
    if (cmd_line == NULL || strlen(cmd_line) == 0) {
        return WARN_NO_CMDS;  // No command provided
    }
    
    char *token;       // Stores the current tokenized command
    char *location;    // Stores the remaining part of cmd_line for strtok_r()
    int count = 0;     // Counter to track number of commands processed

    // Tokenize the command line using the pipe '|' as the delimiter
    token = strtok_r(cmd_line, PIPE_STRING, &location);
    while (token != NULL) {
        // Trim leading spaces
        while (*token == SPACE_CHAR) token++; 

        // Trim trailing spaces
        char *end = token + strlen(token) - 1;
        while (end > token && *end == SPACE_CHAR) *end-- = '\0';

        // If the command is empty after trimming, return a warning
        if (strlen(token) == 0) {
            return WARN_NO_CMDS; 
        }

        // Ensure we do not exceed the maximum allowed commands
        if (count >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        // Find the first space in the command to separate exe and args
        char *arg_ptr = strchr(token, SPACE_CHAR);
        if (arg_ptr) {
            *arg_ptr = '\0';   // Null-terminate the executable name
            arg_ptr++;         // Move to the start of arguments

            // Copy the arguments safely with a size limit
            strncpy(clist->commands[count].args, arg_ptr, ARG_MAX - 1);
            clist->commands[count].args[ARG_MAX - 1] = '\0'; // Ensure null termination
        } else {
            clist->commands[count].args[0] = '\0'; // No arguments
        }

        // Copy the executable name safely with a size limit
        strncpy(clist->commands[count].exe, token, EXE_MAX - 1);
        clist->commands[count].exe[EXE_MAX - 1] = '\0'; // Ensure null termination
        
        count++;  // Increment command count

        // Get the next command token
        token = strtok_r(NULL, PIPE_STRING, &location);
    }

    clist->num = count; // Store the total number of commands parsed
    return OK;
}