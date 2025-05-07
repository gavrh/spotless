#ifndef SPOTLESS_PHONE_HPP
#define SPOTLESS_PHONE_HPP

#include <array>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

namespace phone {

typedef enum {
    SUCCESS =       200,
    EXPIRED_TOKEN = 401,
    FAILED =        404,
} ResponseCode;

class Phone {

private:
    CURL*                                           handler;
    std::unordered_map<std::string, std::string>    headers;
    std::string                                     access_token;

public:
    ResponseCode                                    code;
    nlohmann::json                                  data;

    Phone();
    ~Phone();

    void Perform(std::string url, bool use_auth, std::initializer_list<std::array<std::string, 2>> fields);
    void SetHeader(std::string key, std::string value);
    void SetToken(std::string new_access_token);

private:

    struct curl_slist* HeaderMapToList();
    std::string UrlEncode(std::initializer_list<std::array<std::string, 2>> fields);
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

};

}

#endif // SPOTLESS_PHONE_HPP
