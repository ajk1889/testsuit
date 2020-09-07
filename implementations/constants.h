#ifndef TESTSUIT_CONSTANTS_H
#define TESTSUIT_CONSTANTS_H

#include <map>
#include <string>

typedef unsigned long long ULong;
constexpr auto KB = 1024U;
constexpr auto MB = 1024UL * 1024UL;
constexpr auto GB = 1024ULL * 1024ULL * 1024ULL;
constexpr auto MAX_HEADER_SIZE = 8U * 1024U;
constexpr auto BUFFER_SIZE = 2 * 1024U;
struct ResponseCode {
    static constexpr auto Continue = 100;
    static constexpr auto SwitchingProtocols = 101;
    static constexpr auto Processing = 102;
    static constexpr auto OK = 200;
    static constexpr auto Created = 201;
    static constexpr auto Accepted = 202;
    static constexpr auto NonAuthoritativeInformation = 203;
    static constexpr auto NoContent = 204;
    static constexpr auto ResetContent = 205;
    static constexpr auto PartialContent = 206;
    static constexpr auto MultiStatus = 207;
    static constexpr auto AlreadyReported = 208;
    static constexpr auto IMUsed = 226;
    static constexpr auto MultipleChoices = 300;
    static constexpr auto MovedPermanently = 301;
    static constexpr auto Found = 302;
    static constexpr auto SeeOther = 303;
    static constexpr auto NotModified = 304;
    static constexpr auto UseProxy = 305;
    static constexpr auto TemporaryRedirect = 307;
    static constexpr auto PermanentRedirect = 308;
    static constexpr auto BadRequest = 400;
    static constexpr auto Unauthorized = 401;
    static constexpr auto PaymentRequired = 402;
    static constexpr auto Forbidden = 403;
    static constexpr auto NotFound = 404;
    static constexpr auto MethodNotAllowed = 405;
    static constexpr auto NotAcceptable = 406;
    static constexpr auto ProxyAuthenticationRequired = 407;
    static constexpr auto RequestTimeout = 408;
    static constexpr auto Conflict = 409;
    static constexpr auto Gone = 410;
    static constexpr auto LengthRequired = 411;
    static constexpr auto PreconditionFailed = 412;
    static constexpr auto PayloadTooLarge = 413;
    static constexpr auto RequestURITooLong = 414;
    static constexpr auto UnsupportedMediaType = 415;
    static constexpr auto RequestedRangeNotSatisfiable = 416;
    static constexpr auto ExpectationFailed = 417;
    static constexpr auto ImATeapot = 418;
    static constexpr auto MisdirectedRequest = 421;
    static constexpr auto UnProcessableEntity = 422;
    static constexpr auto Locked = 423;
    static constexpr auto FailedDependency = 424;
    static constexpr auto UpgradeRequired = 426;
    static constexpr auto PreconditionRequired = 428;
    static constexpr auto TooManyRequests = 429;
    static constexpr auto RequestHeaderFieldsTooLarge = 431;
    static constexpr auto ConnectionClosedWithoutResponse = 444;
    static constexpr auto UnavailableForLegalReasons = 451;
    static constexpr auto ClientClosedRequest = 499;
    static constexpr auto InternalServerError = 500;
    static constexpr auto NotImplemented = 501;
    static constexpr auto BadGateway = 502;
    static constexpr auto ServiceUnavailable = 503;
    static constexpr auto GatewayTimeout = 504;
    static constexpr auto HTTPVersionNotSupported = 505;
    static constexpr auto VariantAlsoNegotiates = 506;
    static constexpr auto InsufficientStorage = 507;
    static constexpr auto LoopDetected = 508;
    static constexpr auto NotExtended = 510;
    static constexpr auto NetworkAuthenticationRequired = 511;
    static constexpr auto NetworkConnectTimeoutError = 599;
    static const std::map<uint, std::string> text;
};
#define SERVER_NAME "TestSuit 0.0.0"
#endif //TESTSUIT_CONSTANTS_H
