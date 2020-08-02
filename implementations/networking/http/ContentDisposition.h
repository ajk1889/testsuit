#ifndef TESTSUIT_CONTENTDISPOSITION_H
#define TESTSUIT_CONTENTDISPOSITION_H

#include <string>
#include <stdexcept>

using std::string;

struct ContentDisposition {
    constexpr static auto TYPE_FORM_DATA = "form-data";
    constexpr static auto KEY = "Content-Disposition";
    string type;
    string fileName;
    string name;

    ContentDisposition(const string &disposition) {
        const auto separatorPos = disposition.find(';');
        if (separatorPos == string::npos)
            throw std::runtime_error("Invalid Content-Disposition " + disposition);
        type = disposition.substr(0, separatorPos);
        const auto nameStartPos = disposition.find("name=\"", separatorPos) + 6;
        const auto nameEndPos = disposition.find('"', nameStartPos);
        name = disposition.substr(nameStartPos, nameEndPos - nameStartPos);
        const auto fileStartPos = disposition.find("filename=\"", nameEndPos) + 10;
        const auto fileEndPos = disposition.find('"', fileStartPos);
        fileName = disposition.substr(fileStartPos, fileEndPos - fileStartPos);
    }
};

#endif //TESTSUIT_CONTENTDISPOSITION_H
