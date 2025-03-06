1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

The implementation uses waitpid() on all forked child processes in the execute_pipeline() and exec_cmd() functions. The parent process calls waitpid() for each child to ensure they terminate before proceeding. If waitpid() were not called, zombie processes would accumulate because the parent wouldn't clean up terminated children. Additionally, the shell might return to accepting input too soon, causing race conditions or unexpected behavior.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

Pipes have two ends: a read end and a write end. When a child process redirects its input/output using dup2(), the original pipe file descriptors should be closed to prevent resource leaks. Leaving pipes open can cause issues such as:
- The writing process not receiving an EOF signal because the read end is still open elsewhere.
- Resource exhaustion from too many open file descriptors
- Potential deadlocks if a process waits indefinitely for input from a pipe that remains open but unused.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

cd modifies the shell’s working directory using chdir(), which affects the process executing it. If cd were implemented as an external command, a child process would change its directory, but the parent shell (which remains in control) wouldn’t be affected. This means the directory change would be lost when the child exits. Built-in implementation ensures that the shell process itself updates its working directory.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

Instead of a fixed-size array (CMD_MAX), dynamically allocate memory for storing command structures:
- Use realloc() to expand the array as more commands are parsed.
- Use linked lists instead of arrays to store commands dynamically.

The trade-offs to consider are:
- Pros: Greater flexibility, supporting an arbitrary number of commands.
- Cons: Higher memory management complexity, possible fragmentation issues, and increased execution overhead due to dynamic allocation.
