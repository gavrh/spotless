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

using namespace ftxui;
using json = nlohmann::json;

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

    int x = 10, y = 5;
    int max_width = 0, max_height = 0;

   auto screen = ScreenInteractive::Fullscreen();

    auto component = Renderer([&] {
        // Get current terminal size
        max_width = screen.dimx();
        max_height = screen.dimy();

        // Ensure "X" stays within bounds
        x = std::clamp(x, 0, max_width - 2);
        y = std::clamp(y, 0, max_height - 2);

        // Create a dynamic grid based on terminal size
        std::vector<Element> rows;
        for (int row = 0; row < max_height - 1; row++) {
            std::string line(max_width - 1, ' ');
            if (row == y) line[x] = 'X';  // Place "X"
            rows.push_back(text(line));
        }

        return vbox(rows) | border;
    });

    // Catch events for movement and resizing
    component = CatchEvent(component, [&](Event event) {
        if (event == Event::Character('q')) {
            screen.ExitLoopClosure()();
            return true;
        }
        if (event == Event::ArrowLeft && x > 0) x--;
        if (event == Event::ArrowRight && x < max_width - 2) x++;
        if (event == Event::ArrowUp && y > 0) y--;
        if (event == Event::ArrowDown && y < max_height - 2) y++;

        return true;  // Event handled, force UI update
    });

    screen.Loop(component);

    return EXIT_SUCCESS;
}
