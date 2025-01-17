#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int count_words(char *, int, int);
//add additional prototypes here
void reverse_string(char *, int);
void word_print(char *, int);

// Function to copy characters from user_str to buff, handling spaces and buffer size constraints
int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    if (!buff || !user_str) {
        return -2; // Error: Null pointer passed
    }

    int buff_index = 0;
    int user_index = 0;
    char last_char = 0;
    
    // Traverse through the user string until the null-terminator is reached
    while (user_str[user_index] != '\0') {
        if (buff_index >= len) {
            return -1; // Error: User string exceeds buffer size
        }

        // If the current character is not a space or tab, copy it to the buffer
        if (user_str[user_index] != ' ' && user_str[user_index] != '\t') {
            buff[buff_index++] = user_str[user_index];
            last_char = user_str[user_index];
        } else if (last_char != ' ') {
            buff[buff_index++] = ' ';   
            last_char = ' ';           
        }

        user_index++;
    }

    // Fill remaining buffer with '.' characters
    while (buff_index < len) {
        buff[buff_index++] = '.';
    }

    return user_index; // Return the length of the user string
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    //YOU MUST IMPLEMENT

    // Validate inputs: buffer pointer must not be NULL, and lengths must be positive
    if (!buff || len <= 0 || str_len <= 0) {
        return -2; // Error: Invalid input
    }

    int word_count = 0;
    int in_word = 0; // Flag to indicate whether we're currently inside a word

    for (int i = 0; i < str_len; i++) {
        if (buff[i] != ' ') {
            if (!in_word) { // adds 1 to word_count only when the previous character was a space
                word_count++; 
                in_word = 1;
            }
        } else {
            in_word = 0;
        }
    }

    return word_count;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS
void reverse_string(char *buff, int str_len) {
    if (!buff || str_len <= 0) {
        printf("Error reversing string\n");
        return;
    }

    printf("Reversed String: ");
    // Loop through the string in reverse order
    for (int i = str_len - 1; i >= 0; i--) {
        putchar(buff[i]);
    }
    putchar('\n');
}

void word_print(char *buff, int str_len) {
    if (!buff || str_len <= 0) {
        printf("Error printing words\n");
        return;
    }

    printf("Word Print\n----------\n");
    int word_index = 1;
    int char_count = 0;

    // Loop through each character of the string
    for (int i = 0; i < str_len; i++) {
        if (buff[i] != ' ') { // If the current character is not a space print the character
            putchar(buff[i]);
            char_count++;
        } else if (char_count > 0) { // If a space is encountered and a word has been counted 
            printf(" (%d)\n", char_count);
            char_count = 0;
            printf("%d. ", ++word_index); 
        }
    }

    // Print the length of the last word if it wasn't followed by a space
    if (char_count > 0) {
        printf(" (%d)\n", char_count);
    }
}

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      The use of short-circuit evaluation ensures that argv[1] is only dereferenced if 
    //      argc is 2 or greater, making the code safe from accessing out-of-bounds memory
    if ((argc < 2) || (*argv[1] != '-')) {
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    // It is validate that the program was provided with the 
    // required number of command-line arguments before proceeding
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    buff = (char *)malloc(BUFFER_SZ);
    if (!buff) {
        printf("Error allocating memory\n");
        exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d\n", user_str_len);
        free(buff);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                free(buff);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;
        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        case 'r':
            reverse_string(buff, user_str_len);
            break;
        case 'w':
            word_print(buff, user_str_len);
            break;
        case 'x':
            printf("Not Implemented!\n");
            break;
        default:
            usage(argv[0]);
            free(buff);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  

//          While the buffer size is fixed at 50 bytes in this particular implementation, future 
//          changes might modify the buffer size, or the program may require buffers of different 
//          sizes. Passing the size explicitly makes the code adaptable to such changes.
//          If you decide to reuse your helper functions in a different program with buffers 
//          of varying sizes, having both the buffer and its length as arguments ensures the 
//          functions will work without modification.
