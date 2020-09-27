#ifndef TESTSUIT_URLMAPOBJECT_H
#define TESTSUIT_URLMAPOBJECT_H

#include <string>
#include <vector>
#include <regex>
#include <memory>
#include "json/json.hpp"

using std::string;
using std::vector;
using std::regex;
using std::shared_ptr;
using std::make_shared;

struct ArgDetails {
    string arg;
    string description;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ArgDetails, arg, description);
};

struct UrlMapObject {
    string path;
    vector<string> command;
    vector<ArgDetails> allowedArgs;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(UrlMapObject, path, command, allowedArgs);

    const regex &getPathRegex() {
        if (pathRegex) return *pathRegex;
        pathRegex = make_shared<regex>(path);
        return *pathRegex;
    }

private:
    shared_ptr<regex> pathRegex;
};


#endif //TESTSUIT_URLMAPOBJECT_H
