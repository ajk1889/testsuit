#include "../../../../server/Server.h"
#include "FileResponse.h"

Socket &FileResponse::writeTo(Socket &socket) {
    HttpResponse::writeTo(socket);
    char buffer[BUFFER_SIZE + 1];
    uint_least64_t totalBytesRead = 0;
    auto length = end - begin + 1;
    using std::chrono_literals::operator ""ms;
    auto timeDiff = 33ms;
    auto thisTimeSectionEndTime = preciseNow() + timeDiff;
    auto thisSectionReadCount = 0;
    while (totalBytesRead < length && stream.read(buffer, min(BUFFER_SIZE, length - totalBytesRead))) {
        auto readLimitPerTimeSection = params.maxDownloadSpeed * 1024 / (1000 / 33);
        auto bytesRead = stream.gcount();
        auto now = preciseNow();
        if (thisTimeSectionEndTime < now) {
            thisTimeSectionEndTime = thisTimeSectionEndTime + timeDiff;
            if (thisTimeSectionEndTime <= now)
                thisTimeSectionEndTime = now + timeDiff;
            thisSectionReadCount = 0;
        }
        thisSectionReadCount += bytesRead;
        if (thisSectionReadCount >= readLimitPerTimeSection) {
            auto sleepTime = thisTimeSectionEndTime - now;
            std::this_thread::sleep_for(sleepTime);
            thisTimeSectionEndTime += timeDiff;
            thisSectionReadCount -= readLimitPerTimeSection;
            if (readLimitPerTimeSection <= thisSectionReadCount)
                thisSectionReadCount = 0;
        }
        if (readLimitPerTimeSection - thisSectionReadCount < 1)
            thisSectionReadCount = 0;

        totalBytesRead += bytesRead;
        socket.write(buffer, bytesRead);
    }
    return socket;
}
