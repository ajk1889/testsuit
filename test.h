//
// Created by ajk on 08/09/20.
//

#ifndef TESTSUIT_TEST_H
#define TESTSUIT_TEST_H

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <cstring>

#define PIPE_READ 0
#define PIPE_WRITE 1

int createChild(const char *szCommand, char *const aArguments[], const char *szMessage) {
    int aStdinPipe[2];
    int aStdoutPipe[2];
    int nChild;
    char nChar[1025];
    int nResult;

    if (pipe(aStdinPipe) < 0) {
        perror("allocating pipe for child input redirect");
        return -1;
    }
    if (pipe(aStdoutPipe) < 0) {
        close(aStdinPipe[PIPE_READ]);
        close(aStdinPipe[PIPE_WRITE]);
        perror("allocating pipe for child output redirect");
        return -1;
    }

    nChild = fork();
    if (0 == nChild) {
        // child continues here

        // redirect stdin
        if (dup2(aStdinPipe[PIPE_READ], STDIN_FILENO) == -1) {
            exit(errno);
        }

        // redirect stdout
        if (dup2(aStdoutPipe[PIPE_WRITE], STDOUT_FILENO) == -1) {
            exit(errno);
        }

        // redirect stderr
        if (dup2(aStdoutPipe[PIPE_WRITE], STDERR_FILENO) == -1) {
            exit(errno);
        }

        // all these are for use by parent only
        close(aStdinPipe[PIPE_READ]);
        close(aStdinPipe[PIPE_WRITE]);
        close(aStdoutPipe[PIPE_READ]);
        close(aStdoutPipe[PIPE_WRITE]);

        // run child process image
        // replace this with any exec* function find easier to use ("man exec")
        nResult = execvp(szCommand, aArguments);
        // if we get here at all, an error occurred, but we are in the child
        // process, so just exit
        return nResult;
    } else if (nChild > 0) {
        // parent continues here

        // close unused file descriptors, these are for child only
        close(aStdinPipe[PIPE_READ]);
        close(aStdoutPipe[PIPE_WRITE]);

        // Include error check here
        if (nullptr != szMessage) {
            write(aStdinPipe[PIPE_WRITE], szMessage, strlen(szMessage));
        }

        // Just a char by char read here, you can change it accordingly
        int n;
        while ((n = read(aStdoutPipe[PIPE_READ], &nChar, 1024)) > 0) {
            write(STDOUT_FILENO, &nChar, n);
            nChar[n] = '\0';
            std::cout << n << ": |" << nChar << '|' << std::endl;
        }

        // done with these in this example program, you would normally keep these
        // open of course as long as you want to talk to the child
        close(aStdinPipe[PIPE_WRITE]);
        close(aStdoutPipe[PIPE_READ]);
    } else {
        // failed to create child
        close(aStdinPipe[PIPE_READ]);
        close(aStdinPipe[PIPE_WRITE]);
        close(aStdoutPipe[PIPE_READ]);
        close(aStdoutPipe[PIPE_WRITE]);
    }
    return nChild;
}

#endif //TESTSUIT_TEST_H
