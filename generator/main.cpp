#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "boost/format.hpp"
#include "Generator.h"
#include "json/json.hpp"

using json = nlohmann::json;
using std::string;
using std::map;
using std::vector;

inline ULong min(ULong a, ULong b) {
    return a > b ? b : a;
}

int main() {
    json data;
    std::cin >> data;
    auto getBegin = 0ULL, getEnd = 100ULL * 1024ULL * 1024ULL;
    auto rangeBegin = 0ULL, rangeEnd = 0ULL;

    auto getParams = data["GET"];
    auto startIter = getParams.find("start");
    auto endIter = getParams.find("end");
    if (startIter != getParams.end())
        getBegin = std::stoull(static_cast<string>(*startIter));
    if (endIter != getParams.end())
        getEnd = std::stoull(static_cast<string>(*endIter));

    if (data["HEADERS"].contains("Range")) {
        string range = data["HEADERS"]["Range"][0];
        rangeBegin = std::stoull(range.substr(6, range.find('-') - 6));
        if (range.find('-') != range.length() - 1)
            rangeEnd = std::stoull(range.substr(range.find('-') + 1));
    }

    const auto actualBegin = getBegin + rangeBegin;
    if (!rangeEnd) rangeEnd = getEnd - getBegin - 1;
    Generator generator(actualBegin);
    map<string, vector<string>> headers;
    headers["Content-Type"] = {"application/octet-stream"};
    headers["Content-Disposition"] = {"attachment; filename=\"numbers.txt\""};
    headers["Content-Range"] = {(boost::format("bytes %d-%d/%d") % rangeBegin % rangeEnd % (getEnd - getBegin)).str()};
    json meta{
            {"responseCode", 200},
            {"headers",      headers},
            {"length",       getEnd - getBegin},
            {"data",         "inline"}
    };
    std::cout << meta.dump() << "\n" << std::endl;
    constexpr auto BUFFER_SIZE = 1024U * 10U;
    const auto totalBytes = getEnd - actualBegin;
    auto read = 0ULL;
    char buffer[BUFFER_SIZE + 1];
    while (read < totalBytes) {
        auto bytesToWrite = min(BUFFER_SIZE, totalBytes - read);
        generator.read(buffer, bytesToWrite);
        read += bytesToWrite;
        buffer[bytesToWrite] = '\0';
        std::cout << buffer;
    }
    std::cout.flush();
    return 0;
}
