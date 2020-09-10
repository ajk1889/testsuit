#include "Process.h"

shared_ptr<Socket> Process::run(const char *input) {
    if (pipe(aStdinPipe) < 0) {
        perror("allocating pipe for child input redirect");
        return shared_ptr<Socket>();
    }
    if (pipe(aStdoutPipe) < 0) {
        close(aStdinPipe[PIPE_READ]);
        close(aStdinPipe[PIPE_WRITE]);
        perror("allocating pipe for child output redirect");
        return shared_ptr<Socket>();
    }

    int nChild = fork();
    if (0 == nChild) {
        // child continues here
        if (dup2(aStdinPipe[PIPE_READ], STDIN_FILENO) == -1) exit(errno);
        if (dup2(aStdoutPipe[PIPE_WRITE], STDOUT_FILENO) == -1) exit(errno);
        if (dup2(aStdoutPipe[PIPE_WRITE], STDERR_FILENO) == -1) exit(errno);

        close(aStdinPipe[PIPE_READ]);
        close(aStdinPipe[PIPE_WRITE]);
        close(aStdoutPipe[PIPE_READ]);
        close(aStdoutPipe[PIPE_WRITE]);
        exit(execvp(command[0], const_cast<char *const *>(args)));
    } else if (nChild > 0) {
        // parent continues here
        close(aStdinPipe[PIPE_READ]);
        close(aStdoutPipe[PIPE_WRITE]);
        if (nullptr != input)
            write(aStdinPipe[PIPE_WRITE], input, strlen(input));
        return make_shared<Socket>(aStdoutPipe[PIPE_READ], nullptr);
    } else {
        perror("Could not fork");
        close(aStdinPipe[PIPE_READ]);
        close(aStdinPipe[PIPE_WRITE]);
        close(aStdoutPipe[PIPE_READ]);
        close(aStdoutPipe[PIPE_WRITE]);
    }
    return shared_ptr<Socket>();
}

void Process::test() {
    Process process = {"python3", "-c", "print(int(input())**237)"};
    char data[1024];
    data[0] = '\0';
    auto socket = process.run("1820\n");
    if (socket) data[socket->read(data, 1023)] = '\0';
    else std::cout << "Null socket" << std::endl;
    std::cout << data << std::endl;
}
