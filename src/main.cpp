#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <cstddef>

using json = nlohmann::json;

size_t write_callback(char *ptr, size_t size, size_t nmemb, std::string *data) {
    size_t total_size = size * nmemb;
    data->append(ptr, total_size);
    return total_size;
}

int main(void) {

    std::cout << std::setw(4) << json::meta() << std::endl;


    ftxui::Element document =
        ftxui::hbox({
            ftxui::text("left") | ftxui::border,
            ftxui::text("middle") | ftxui::border | ftxui::flex,
            ftxui::text("right") | ftxui::border,
        });

    auto screen = ftxui::Screen::Create(
        ftxui::Dimension::Full(),
        ftxui::Dimension::Fit(document)
    );
    ftxui::Render(screen, document);
    screen.Print();

    CURL *curl;
    CURLcode res;
    std::string response_data;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.github.com/users/gavrh");
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Spotless");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            json d = json::parse(response_data);
            std::cout << d.dump(4) << std::endl;
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    return EXIT_SUCCESS;
}
