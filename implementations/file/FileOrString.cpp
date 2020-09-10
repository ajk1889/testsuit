#include "FileOrString.h"
#include "../../server/Server.h"

FileOrString FileOrString::readFrom(Socket &socket, string &boundary) {
    FileOrString fileOrString;
    auto &data = fileOrString.data;
    data.append(readUntilMatch(socket, boundary, BYTES_TO_STORE_IN_MEMORY));
    if (data.rfind(boundary) == string::npos) {
        string filePath = socket.server->params.tempDir + "/" + getRandomString(10);
        std::ofstream op(filePath);
        while (data.rfind(boundary) == string::npos) {
            op.write(data.c_str(), data.length());
            data.clear();
            data.append(readUntilMatch(socket, boundary, BYTES_TO_STORE_IN_MEMORY));
        }
        op.write(data.c_str(), data.length() - boundary.length());
        op.close();
        data = std::move(filePath);
        fileOrString.isFile = true;
    } else data = data.substr(0, data.length() - boundary.length());
    return std::move(fileOrString);
}

string FileOrString::getRandomString(uint len) {
    const static string chars = "1234567890abcdefghijklmnopqrstuvwxyz";
    const auto charsLen = chars.length();
    string randStr;
    randStr.reserve(len);
    while (len-- > 0)
        randStr.push_back(chars[random() % charsLen]);
    return randStr;
}
