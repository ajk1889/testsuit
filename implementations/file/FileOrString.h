//
// Created by ajk on 01/08/20.
//

#ifndef TESTSUIT_FILEORSTRING_H
#define TESTSUIT_FILEORSTRING_H

#include <string>
#include <fstream>
#include "../networking/Socket.h"
#include "../../server/Server.h"

using std::string;

class FileOrString {
    string data;
    bool isFile;

    void readFrom(Socket &socket, string &boundary) {
        data.append(readUntilMatch(socket, boundary, 2 * KB));
        if (data.find_last_of(boundary) == string::npos) {
            std::ofstream op(socket.server->params.sharedPath + "/a.txt");
        }
        while (data.find_last_of(boundary) == string::npos) {

        }
    }
};


#endif //TESTSUIT_FILEORSTRING_H
