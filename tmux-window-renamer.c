#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

// Wrapper for pidfd_open syscall
static int pidfd_open(pid_t pid, unsigned int flags) {
    return syscall(SYS_pidfd_open, pid, flags);
}

int main(int argc, char **argv) {
    // We want to wait for our actual parent pid
    pid_t targetPid = getppid();

    // Check if hostname given and nothing else
    if (argc != 2) {
        printf("Usage: %s [hostname]", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Check if we're inside a tmux session
    if (strcmp(getenv("TERM_PROGRAM"), "tmux") != 0) {
        exit(EXIT_SUCCESS);
    }

    // Rename the current tmux window to the given hostname
    char tmuxWindowRenameCommand[1024];
    sprintf(tmuxWindowRenameCommand, "tmux rename-window '%s'", argv[1]);
    system(tmuxWindowRenameCommand);

    // Fork the process
    pid_t pid = fork();

    if (pid < 0) {
        // Forking failed
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // This is the child process
        // Detach from the parent process
        setsid();

        int pidfd, ready;
        struct pollfd pollfd;

        // Child process waits for original parent PID to finish
        pidfd = pidfd_open(targetPid, 0);
        if (pidfd == -1) {
            // Could not open the parent process for polling
            exit(EXIT_FAILURE);
        }

        pollfd.fd = pidfd;
        pollfd.events = POLLIN;

        // Poll the parent pid fd
        ready = poll(&pollfd, 1, -1);
        if (ready == -1) {
            // Polling itself failed
            exit(EXIT_FAILURE);
        }

        close(pidfd);

        // Re-enable the automatic renaming on this tmux window
        system("tmux set-window-option automatic-rename 'on' 1>/dev/null");
        exit(EXIT_SUCCESS);
    }
}
