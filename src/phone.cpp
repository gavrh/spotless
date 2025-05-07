#include <cstdlib>
#include <curl/curl.h>
#include <iostream>
#include <phone.hpp>
#include <sstream>

namespace phone {

Phone::Phone() {
    this->handler = curl_easy_init();
    if (!this->handler) {
        std::cerr << "failed to init curl" << std::endl;
        std::exit(1);
    }
    this->SetHeader("Content-Type", "application/x-www-form-urlencoded");
}

Phone::~Phone() {
    // curl_easy_cleanup(this->handler);
}

void Phone::Perform(std::string url, bool use_auth, std::initializer_list<std::array<std::string, 2>> fields) {

    std::string encoded = this->UrlEncode(fields);
    struct curl_slist* headers = this->HeaderMapToList();
    if (use_auth) curl_slist_append(headers, ("Authorization: Bearer " + this->access_token).c_str());
    std::string buffer;

    curl_easy_setopt(this->handler, CURLOPT_URL, url.c_str());
    curl_easy_setopt(this->handler, CURLOPT_HTTPHEADER, headers);
    // if (fields.size() != 0) curl_easy_setopt(this->handler, CURLOPT_POSTFIELDS, encoded.c_str());
    curl_easy_setopt(this->handler, CURLOPT_WRITEFUNCTION, this->WriteCallback);
    curl_easy_setopt(this->handler, CURLOPT_WRITEDATA, &buffer);

    CURLcode res = curl_easy_perform(this->handler);
    try {
        this->data = nlohmann::json::parse(buffer);
        if (this->data.contains("error")) {
            this->code = static_cast<ResponseCode>(this->data["error"]["status"].get<int>());
        } else {
            this->code = SUCCESS;
        }
    } catch (const std::exception &e) {
    }

    curl_slist_free_all(headers);
}

void Phone::SetHeader(std::string key, std::string value) {
    this->headers[key] = value;
}

void Phone::SetToken(std::string token) {
    this->access_token = token;
}

struct curl_slist* Phone::HeaderMapToList() {
    struct curl_slist* headers = nullptr;
    for (const auto& [key, value] : this->headers) {
        std::string header_line = key + ": " + value;
        headers = curl_slist_append(headers, header_line.c_str());
    }
    return headers;
}

std::string Phone::UrlEncode(std::initializer_list<std::array<std::string, 2>> fields) {
    std::ostringstream encoded;
    bool first = true;

    for (const auto& pair : fields) {
        char* key = curl_easy_escape(this->handler, pair[0].c_str(), 0);
        char* val = curl_easy_escape(this->handler, pair[1].c_str(), 0);

        if (!first) encoded << "&";
        encoded << key << "=" << val;

        first = false;
        curl_free(key);
        curl_free(val);
    }

    return encoded.str();
}

size_t Phone::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

}
