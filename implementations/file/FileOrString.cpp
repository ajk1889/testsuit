#include "FileOrString.h"
#include "../../server/Server.h"

FileOrString FileOrString::readFrom(Socket &socket, const string &boundary) {
    FileOrString fileOrString;
    auto &data = fileOrString.data;
    data.append(readUntilMatch(socket, boundary, BYTES_TO_STORE_IN_MEMORY));
    if (data.rfind(boundary) == string::npos) {
        const auto boundaryLen = boundary.length();
        string lastFew(data, data.length() - (boundaryLen - 1));

        string filePath = params.getTempDir() + "/" + getRandomString(10);
        std::ofstream op(filePath);
        char buffer[BUFFER_SIZE];
        ssize_t bytesRead;
        while ((bytesRead = socket.read(buffer, BUFFER_SIZE)) > 0) {
            auto terminationPoint = find(buffer, boundary, bytesRead, lastFew);
            if (terminationPoint == -1) {
                data.append(buffer, bytesRead);
                fill(buffer, lastFew, bytesRead);
            } else {
                data.append(buffer, terminationPoint);
                socket.unread(buffer + terminationPoint, bytesRead - terminationPoint);
                break;
            }
            auto dataLen = data.length();
            if (dataLen > boundaryLen) {
                op.write(data.c_str(), dataLen - boundaryLen);
                data = data.substr(dataLen - boundaryLen);
            }
        }
        auto dataLen = data.length();
        if (dataLen > boundaryLen)
            op.write(data.c_str(), dataLen - boundaryLen);
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

FileOrString FileOrString::readFrom(Socket &socket, uint_least64_t nBytes) {
    FileOrString fileOrString;
    auto &data = fileOrString.data;
    if (nBytes < BYTES_TO_STORE_IN_MEMORY) {
        data.append(readExact(socket, nBytes));
        return std::move(fileOrString);
    }
    data = params.getTempDir() + "/" + getRandomString(10);
    std::ofstream op(data);
    char buffer[BUFFER_SIZE + 1]{};
    ssize_t bytesRead;
    while (nBytes > 0 && (bytesRead = socket.read(buffer, min(BUFFER_SIZE, nBytes))) > -1) {
        op.write(buffer, bytesRead);
        nBytes -= bytesRead;
    }
    op.close();
    fileOrString.isFile = true;
    return std::move(fileOrString);
}
