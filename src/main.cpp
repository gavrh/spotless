#include <chrono>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <iostream>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <cstddef>
#include <thread>
#include <chrono>
#include <CImg.h>

using json = nlohmann::json;
using namespace cimg_library;

size_t write_callback(char *ptr, size_t size, size_t nmemb, std::string *data) {
    size_t total_size = size * nmemb;
    data->append(ptr, total_size);
    return total_size;
}

int main(void) {

    // testing nlohmann_json
    std::cout << std::setw(4) << json::meta() << std::endl;

    // testing libcurl
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

    // testing CImg
    CImg<unsigned char> image(640, 480, 1, 3, 0);
    unsigned char red[] = {255, 0, 0};
    image.draw_text(100, 100, "Hello, USING CIMG HERE!", red);
    image.display("TEST CIMG");

    // testing ftxui
    int dots = 0;
    auto renderer = ftxui::Renderer([&] {
        return ftxui::text(std::string("Spotless") + std::string(dots, '.'));
    });

    auto screen = ftxui::ScreenInteractive::Fullscreen();
    std::thread updater([&] {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            dots = (dots + 1) % 4;
            screen.PostEvent(ftxui::Event::Custom);
        }
    });

    // screen.Loop(renderer);

    return EXIT_SUCCESS;
}
