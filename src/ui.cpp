#include <ftxui/component/component.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <librespot/playback.h>
#include <ui.hpp>

namespace ui {
using namespace ftxui;

std::string repeat_utf8(const std::string& str, int times) {
    std::string out;
    for (int i = 0; i < times; ++i)
        out += str;
    return out;
}

App::App(
    cache::Cache        &cache,
    config::Config      &config,
    spotify::Spotify    &spotify
) : cache(cache), config(config), spotify(spotify) {

    this->cache = cache;
    this->config = config;
    this->spotify = spotify;

    this->spotify.Load(spotify.context.current.uri, false, spotify.context.position_ms);
    mixer_set_volume(this->spotify.mixer, UINT16_MAX * (0.01 * this->spotify.context.volume));

    auto screen = ScreenInteractive::TerminalOutput().Fullscreen();
    auto last_press_time = std::chrono::steady_clock::now();
    const auto delay = std::chrono::milliseconds(250);
    // Menu entries
    std::vector<std::string> horizontal_entries = {"Tracks", "Playlists", "Help"};
    int selected_horizontal = 0;

    std::vector<std::string> vertical_entries = {
        "Item A", "Item B", "Item C", "Item D", "Item E", "Item F", "Item G"
        ,"Item A", "Item B", "Item C", "Item D", "Item E", "Item F", "Item G"
        ,"Item A", "Item B", "Item C", "Item D", "Item E", "Item F", "Item G"
        ,"Item A", "Item B", "Item C", "Item D", "Item E", "Item F", "Item G"
        ,"Item A", "Item B", "Item C", "Item D", "Item E", "Item F", "Item G"
        //,"Item A", "Item B", "Item C", "Item D", "Item E", "Item F", "Item G"
        //,"Item A", "Item B", "Item C", "Item D", "Item E", "Item F", "Item G"
        //,"Item A", "Item B", "Item C", "Item D", "Item E", "Item F", "Item G"
        //,"Item A", "Item B", "Item C", "Item D", "Item E", "Item F", "Item G"
        //,"Item A", "Item B", "Item C", "Item D", "Item E", "Item F", "Item G"
        //,"Item A", "Item B", "Item C", "Item D", "Item E", "Item F", "Item G"
        //,"Item A", "Item B", "Item C", "Item D", "Item E", "Item F", "Item G"
        //,"Item A", "Item B", "Item C", "Item D", "Item E", "Item F", "Item G"
    };
    int selected_vertical = 2;

    // Custom horizontal menu
    Component horizontal_menu = Renderer([&] {
        Elements items;
        for (size_t i = 0; i < horizontal_entries.size(); ++i) {
            bool selected = (int)i == selected_horizontal;
            items.push_back(
                center(text(horizontal_entries[i])) |
                (selected ? bgcolor(Color::Green) | color(Color::White) : nothing) |
                flex
            );
        }
        return hbox(std::move(items)) | xflex;
    });

    // Custom vertical menu
    // Component vertical_menu = Renderer([&] {
    //     Elements items;
    //     for (size_t i = 0; i < vertical_entries.size(); ++i) {
    //         bool selected = (int)i == selected_vertical;
    //         items.push_back(
    //             text(vertical_entries[i]) |
    //             (selected ? bgcolor(Color::Green) | color(Color::White) : nothing)
    //         );
    //     }
    //     return vbox(std::move(items)) | yflex | xflex;
    // });

    int scroll_offset = 0;

    Component vertical_menu = Renderer([&]() {
        // Get the available vertical space from the component
        int visible_lines = screen.dimy()-4;

        // Adjust scroll to keep selection in view
        if (selected_vertical < scroll_offset)
            scroll_offset = selected_vertical;
        else if (selected_vertical >= scroll_offset + visible_lines)
            scroll_offset = selected_vertical - visible_lines + 1;

        Elements items;
        for (int i = scroll_offset; i < scroll_offset + visible_lines && i < (int)vertical_entries.size(); ++i) {
            bool is_selected = i == selected_vertical;
            auto item = text(vertical_entries[i]) |
                (is_selected ? bgcolor(Color::Green) | color(Color::White) : nothing);
            items.push_back(item);
        }

        return vbox(std::move(items));
    });

    // Combine all components
    Component main = Container::Vertical({
        horizontal_menu,
        vertical_menu,
    });

    std::string top_text = "Library of " + this->spotify.user.display_name;
    std::string time_elapsed = "0:00";

    std::atomic<bool> running = true;
    Component renderer = CatchEvent(
        Renderer(
            main,
            [&] {
                return vbox({
                    text(top_text) | center | color(Color::Green) | size(HEIGHT, EQUAL, 1) | xflex,
                    horizontal_menu->Render() | size(HEIGHT, EQUAL, 1) | xflex,
                    vertical_menu->Render() | flex,
                    this->TimeBar(screen)->Render(),
                    this->CurrentBar()->Render(), 
                }) | flex;
            }), [&](Event event) {
            if (event == Event::Special("tick")) {
                PlayerEvent* player_event;
                if (player_channel_poll(this->spotify.player_channel, player_event)) {
                    if (player_event->event == PLAYER_EVENT_TIME_TO_PRELOAD_NEXT_TRACK) {
                        this->spotify.Preload("spotify:track:6hxowqRsDm1fsm00y2eHJP");
                    } else if (player_event->event == PLAYER_EVENT_END_OF_TRACK) {
                        this->spotify.Load("spotify:track:6hxowqRsDm1fsm00y2eHJP", true, 0);
                    } else if (player_event->event == PLAYER_EVENT_POSITION_CHANGED) {
                        spotify.context.position_ms = player_event->data.position_changed.position_ms;
                    }
                }
                return true;
            }
            if (event == Event::Character('j')) {
                selected_vertical = selected_vertical == vertical_entries.size()-1 ? selected_vertical : ++selected_vertical;
                return true;
            }
            if (event == Event::Character('k')) {
                selected_vertical = selected_vertical == 0 ? selected_vertical : --selected_vertical;
                return true;
            }
            if (event == Event::Character('l')) {
                selected_horizontal = selected_horizontal == horizontal_entries.size()-1 ? selected_horizontal : ++selected_horizontal;
                return true;
            }
            if (event == Event::Character('h')) {
                selected_horizontal = selected_horizontal == 0 ? selected_horizontal : --selected_horizontal;
                return true;
            }
            if (event == Event::Character('P')) {
                if (spotify.context.is_playing) {
                    player_pause(this->spotify.player);
                } else {
                    player_play(this->spotify.player);
                }
                spotify.context.is_playing = !spotify.context.is_playing;
                return true;
            }
            if (event == Event::Character('f')) {
                auto now = std::chrono::steady_clock::now();
                if (now - last_press_time > delay) {
                    last_press_time = now;
                    player_seek(spotify.player, spotify.context.position_ms + 1000);
                }
                return true;
            }
            if (event == Event::Character('b')) {
                auto now = std::chrono::steady_clock::now();
                if (now - last_press_time > delay) {
                    last_press_time = now;
                    player_seek(spotify.player, spotify.context.position_ms < 1000 ? 0 : spotify.context.position_ms - 1000);
                }
                return true;
            }
            if (event == Event::Character('F')) {
                auto now = std::chrono::steady_clock::now();
                if (now - last_press_time > delay) {
                    last_press_time = now;
                    player_seek(spotify.player, spotify.context.position_ms + 5000);
                }
                return true;
            }
            if (event == Event::Character('B')) {
                auto now = std::chrono::steady_clock::now();
                if (now - last_press_time > delay) {
                    last_press_time = now;
                    player_seek(spotify.player, spotify.context.position_ms < 5000 ? 0 : spotify.context.position_ms - 5000);
                }
                return true;
            }
            if (event == Event::Character('+')) {
                spotify.context.volume++;
                if (spotify.context.volume > 100) spotify.context.volume = 100;
                mixer_set_volume(this->spotify.mixer, UINT16_MAX * (0.01 * this->spotify.context.volume));
                return true;
            }
            if (event == Event::Character('-')) {
                if (spotify.context.volume == 0) {
                    spotify.context.volume = 0;
                } else {
                    spotify.context.volume--;
                }
                if (spotify.context.volume < 0) spotify.context.volume = 0;
                mixer_set_volume(this->spotify.mixer, UINT16_MAX * (0.01 * this->spotify.context.volume));
                return true;
            }
            if (event == Event::Character(']')) {
                spotify.context.volume+=5;
                if (spotify.context.volume > 100) spotify.context.volume = 100;
                mixer_set_volume(this->spotify.mixer, UINT16_MAX * (0.01 * this->spotify.context.volume));
                return true;
            }
            if (event == Event::Character('[')) {
                if (spotify.context.volume < 5) {
                    spotify.context.volume = 0;
                } else {
                    spotify.context.volume-=5;
                }
                mixer_set_volume(this->spotify.mixer, UINT16_MAX * (0.01 * this->spotify.context.volume));
                return true;
            }
            if (event == Event::Character('q')) {
                running = false;
                screen.Exit();
                return true;
            }
            return false;
        });

    std::thread([&screen, &running]() {
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            screen.PostEvent(Event::Special("tick"));
        }
    }).detach();

    screen.Loop(renderer);

}

App::~App() {}

ftxui::Component App::TimeBar(ftxui::ScreenInteractive &screen) {
    return Renderer([&]() {
        float ratio = static_cast<float>(spotify.context.position_ms) / spotify.context.current.duration_ms;
        int total_width = screen.dimx();

        int filled_len = static_cast<int>(ratio * total_width);
        int empty_len = total_width - filled_len;

        std::string bar =
            repeat_utf8("━", filled_len) +
            repeat_utf8("╌", empty_len);


        return text(bar) | color(Color::Blue);
    });
}

ftxui::Component App::CurrentBar() {
    return Renderer([&]() {
        return hbox({
            text(
                (spotify.context.is_playing
                ? "⏸️" : "▶️")
                + spotify.context.current.artist
                + " - " + spotify.context.current.name
            ),
            text("  ") | xflex,
            text(
                std::to_string(spotify.context.position_ms/1000)
                + "/" + std::to_string(spotify.context.current.duration_ms/1000)
                + " [" + std::to_string(spotify.context.volume)
                + "%%]"
            )
        });
    });
}

}
