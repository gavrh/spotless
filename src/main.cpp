#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>
#include <iomanip>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

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

    return EXIT_SUCCESS;
}
