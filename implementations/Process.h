#ifndef TESTSUIT_PROCESS_H
#define TESTSUIT_PROCESS_H

#include <string>
#include <utility>
#include <vector>
#include "networking/Socket.h"

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <cstring>

#define PIPE_READ 0
#define PIPE_WRITE 1

using std::string;
using std::vector;
using std::make_shared;
using std::shared_ptr;

class Process {
private:
    vector<const char *> command;
    char const **args;
    int aStdinPipe[2]{};
    int aStdoutPipe[2]{};
public:
    Process(const std::initializer_list<const char *> &cmdAndArgs) : command(cmdAndArgs) {
        const auto N = this->command.size();
        args = new char const *[N + 1];
        for (int i = 0; i < N; ++i)
            args[i] = this->command[i];
        args[N] = nullptr;
    }

    shared_ptr<Socket> run(const char *input);

    ~Process() {
        delete[] args;
        close(aStdinPipe[PIPE_READ]);
        close(aStdinPipe[PIPE_WRITE]);
        close(aStdoutPipe[PIPE_READ]);
        close(aStdoutPipe[PIPE_WRITE]);
    }
};


#endif //TESTSUIT_PROCESS_H
