1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**: fgets() is a good choice because it safely handles strings by preventing buffer overflows. Unlike scanf() or gets(), fgets() allows specifying the maximum number of characters to read, ensuring that input does not exceed the allocated buffer size.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  Using malloc() for cmd_buff allows dynamic memory allocation, making it possible to handle varying command lengths efficiently. If a fixed-size array were used, it could either waste memory when handling short commands or fail to accommodate longer commands. By using malloc(), memory can be allocated based on actual needs, and it allows for future expansion or resizing if necessary.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  Trimming leading and trailing spaces ensures that commands are parsed and executed correctly. If leading spaces remain, the command parser might misinterpret input or fail to recognize a valid command. Not trimming spaces could also affect string comparisons and pattern matching when processing commands.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  
    1. Redirecting output to a file: ls > output.txt - This redirects STDOUT from ls to output.txt. The challenge is handling file permissions and ensuring correct file opening modes (e.g., overwriting vs. appending). 
    2. Redirecting input from a file: sort < input.txt - This takes input from input.txt instead of the keyboard. The challenge lies in ensuring the file exists and handling errors gracefully if it does not.
    3. Redirecting STDERR to a file: gcc main.c 2> errors.log - This captures error messages separately from standard output. Challenges include ensuring the correct file descriptor redirection and differentiating between STDOUT and STDERR when necessary.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Redirection is used to send output to files or take input from files, whereas piping (|) connects the output of one command directly into the input of another. For example, ls | grep txt passes the output of ls into grep. Redirection interacts with files, while pipes create temporary connections between processes.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  Keeping STDERR separate from STDOUT ensures that error messages do not interfere with normal command output. This distinction allows scripts and users to process successful output while handling errors differently. For example, if both were merged, parsing structured output (like CSV or JSON) would become difficult due to interleaved error messages.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  Our custom shell should clearly indicate errors by displaying STDERR separately or logging it. However, we should also provide an option to merge STDOUT and STDERR when needed, using redirection like command 2>&1 to combine them. This flexibility allows users to choose whether to handle errors separately or as part of the main output stream.