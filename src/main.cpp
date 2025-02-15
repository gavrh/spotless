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

enum Mode { Normal, Command };

class CommandLineEmulator {
public:
    CommandLineEmulator() : mode(Normal), command("") {}

    // Handles input events
    bool HandleInput(Event event) {
        if (mode == Normal) {
            if (event == Event::Character(':')) {
                mode = Command;  // Switch to Command Mode
                command.clear();
                return true;
            }
        } else if (mode == Command) {
            if (event == Event::Return) {
                ProcessCommand();
                mode = Normal;  // Return to Normal Mode after executing command
                return true;
            } else if (event == Event::Backspace) {
                if (!command.empty()) command.pop_back();
                return true;
            } else if (event == Event::Escape) {
                mode = Normal;
                return true;
            } else if (event.is_character()) {
                command += event.character();
                return true;
            }
        }
        return false; // Event not handled
    }

    // Renders the UI
    Component RenderUI(Component comp) {
        return Renderer([this, comp] {
            std::string mode_str = (mode == Normal) ? "Normal Mode" : "Command Mode: " + command;

            Elements content = {
                comp->Render() | flex,
                text("Command Line Emulator - Mode: " + mode_str),
                separator(),
                text("Press ':' to enter command mode, then type a command like 'quit' or 'help'."),
            };

            // Show the command input box only in Command Mode
            if (mode == Command) {
                content.push_back(text(":" + command));
            }

            return vbox(content);
        });
    }

private:
    void ProcessCommand() {
        if (command == "quit") {
            std::cout << "Exiting program...\n";
            std::exit(0);
        } else if (command == "help") {
            std::cout << "Available commands:\n";
            std::cout << "  quit - Exit the program\n";
            std::cout << "  help - Show this help text\n";
        } else {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }

    Mode mode;
    std::string command;
};


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

    auto screen = ScreenInteractive::Fullscreen();
    CommandLineEmulator cli;

    auto ui = cli.RenderUI(main_container);
    auto component = CatchEvent(ui, [&cli](Event event) {
        return cli.HandleInput(event);
    });

    screen.Loop(component);  // Use a valid Component for event loop
    return 0;

    return EXIT_SUCCESS;
}
