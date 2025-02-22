#include <ftxui/component/event.hpp>
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
#include <string>
#include <vector>
#include <thread>
#include <librespot/playback/player.h>

using namespace ftxui;
using json = nlohmann::json;

size_t write_callback(char *ptr, size_t size, size_t nmemb, std::string *data) {
    size_t total_size = size * nmemb;
    data->append(ptr, total_size);
    return total_size;
}

#if defined(_WIN32) || defined(_WIN64)
    #define REDIRECT_CMD " > nul 2>&1"
#else
    #define REDIRECT_CMD " > /dev/null 2>&1 < /dev/null &"
#endif

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

    // testing ftxui
    auto title = ftxui::Renderer([] {
        return ftxui::hbox({
            ftxui::text("spotless") | ftxui::color(ftxui::Color::MediumSpringGreen) | ftxui::bold,
            ftxui::text(" - Logged in as ") | ftxui::color(ftxui::Color::White),
            ftxui::text("@user") | ftxui::color(ftxui::Color::MediumSpringGreen) | ftxui::bold,
        });
    });

    
    std::vector<std::string> entries1 = {
        "Playlists",
        "Albums",
    };
    std::vector<std::string> entries2 = {
        "Favorites",
        "Other",
    };
    std::vector<std::string> entries3 = {
        "Profile",
        "Settings",
    };
    int selected2 = 0;
    int selected3 = 0;
    auto menu2 = ftxui::Menu(&entries2, &selected2);
    auto menu3 = ftxui::Menu(&entries3, &selected3);
    auto container1 = ftxui::Container::Vertical({
        ftxui::Button("Playlists", [](){}),
        ftxui::Button("Albums", [](){}),
    });
    auto container2 = ftxui::Container::Vertical({
        menu2,
    });
    auto container3 = ftxui::Container::Vertical({
        menu3,
    });

    auto main_container =  ftxui::Container::Horizontal({
        container1 | ftxui::flex | ftxui::border,
        container2 | ftxui::flex | ftxui::border,
        container3| ftxui::flex | ftxui::border,
    });
   auto screen = ScreenInteractive::Fullscreen();

    auto component = Renderer([title, main_container] {
        return vbox({
            title->Render() | center,
            main_container->Render() | flex,
        });
    });

    //component = CatchEvent(component, [&](Event event) {
    //    return true;
    //});


    std::string librespot_cmd = "nohup librespot --backend pulseaudio --cache ~/.config/librespot_cache --name \"spotless\" --bitrate 320 --disable-discovery --autoplay on > /dev/null 2>&1 &";
    system(librespot_cmd.c_str());

    Player* player = player_new();
    std::cout << player_get_volume(player) << std::endl;
    player_play(player);
    player_pause(player);
    // screen.Loop(component);

    return EXIT_SUCCESS;
}
