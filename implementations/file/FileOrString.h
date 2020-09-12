//
// Created by ajk on 01/08/20.
//

#ifndef TESTSUIT_FILEORSTRING_H
#define TESTSUIT_FILEORSTRING_H

#include <string>
#include <fstream>
#include <utility>
#include "json/json.hpp"
#include "../networking/Socket.h"
#include "../networking/http/ContentDisposition.h"

using std::string;

class FileOrString {
    static constexpr auto BYTES_TO_STORE_IN_MEMORY = 2 * KB;
public:
    string data;
    bool isFile = false;

    FileOrString() = default;

    FileOrString(string rawString) : data(std::move(rawString)) {}

    static FileOrString readFrom(Socket &socket, string &boundary);

    static string getRandomString(uint len);

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(FileOrString, data, isFile);
};


#endif //TESTSUIT_FILEORSTRING_H
