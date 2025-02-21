1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  Calling fork() creates a child process that runs the command while the parent process continues executing independently. If execvp() were executed directly in the parent, it would replace the shell entirely. Using fork() ensures that the shell remains active while the child executes the new command.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If fork() fails, it indicates that the system has run out of memory. To handle this, an error message stating "fork failed" is returned.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  execvp() looks for the specified command within the directories listed in the system’s PATH environment variable. For instance, when typing ls, the system searches through these directories to locate and execute the corresponding program.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  The wait() function ensures that the parent process pauses execution until the child process finishes running. Without calling wait(), the child process would become a zombie, consuming system resources unnecessarily.


5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**: WEXITSTATUS() retrieves the exit status of a completed child process, allowing us to determine whether a command was successful or encountered an error. This information is crucial for handling process execution results properly.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  build_cmd_buff() treats quoted strings as single arguments. This is important because, without this handling, quoted text such as "hello world" would be incorrectly split into separate arguments "hello" and "world" instead of preserving it as "hello world".

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  We transitioned from using command_list_t to cmd_buff_t, ensuring quoted strings were processed as single arguments. One challenge was carefully following the instructions for transferring code from dsh_cli.c to dshlib.c, as some details were slightly unclear. Another difficulty involved verifying the parsing logic without disrupting the command structure.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals provide a way for processes to communicate asynchronously, allowing one process to notify another of an event. Unlike other IPC mechanisms, which facilitate direct data exchange, signals only deliver notifications and do not persist after a process terminates.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  
    - SIGKILL: Immediately forces a process to terminate without allowing it to clean up.
    - SIGTERM: Requests a process to terminate gracefully, allowing it to handle cleanup before exiting.
    - SIGINT: Interrupts a running process, typically triggered by pressing Ctrl+C.


- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**: SIGSTOP suspends a process entirely and cannot be caught or ignored, unlike SIGINT. This is because, while SIGINT allows a process to handle termination gracefully, SIGSTOP enforces an immediate pause, preventing the process from executing further until explicitly resumed with SIGCONT.
