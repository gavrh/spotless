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

App::App(
    cache::Cache        &cache,
    config::Config      &config,
    spotify::Spotify    &spotify
) : cache(cache), config(config), spotify(spotify) {

    this->cache = cache;
    this->config = config;
    this->spotify = spotify;

    this->spotify.Load("spotify:track:55rXApL8KBessVIpJpH4Fw", true, 45000);

    // Menu entries
    std::vector<std::string> horizontal_entries = {"Tracks", "Playlists", "Help"};
    int selected_horizontal = 0;

    std::vector<std::string> vertical_entries = {
        "Item A", "Item B", "Item C", "Item D", "Item E", "Item F", "Item G"
    };
    int selected_vertical = 2;

    // Custom horizontal menu
    Component horizontal_menu = CatchEvent(Renderer([&] {
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
    }), [&](Event event) {
                                           if (event == Event::ArrowLeft && selected_horizontal > 0) {
                                           selected_horizontal--;
                                           return true;
                                           }
                                           if (event == Event::ArrowRight && selected_horizontal < (int)horizontal_entries.size() - 1) {
                                           selected_horizontal++;
                                           return true;
                                           }
                                           return false;
                                           });

    // Custom vertical menu
    Component vertical_menu = CatchEvent(Renderer([&] {
        Elements items;
        for (size_t i = 0; i < vertical_entries.size(); ++i) {
            bool selected = (int)i == selected_vertical;
            items.push_back(
                text(vertical_entries[i]) |
                (selected ? bgcolor(Color::Green) | color(Color::White) : nothing)
            );
        }
        return vbox(std::move(items)) | yflex | xflex;
    }), [&](Event event) {
                                         if (event == Event::ArrowUp && selected_vertical > 0) {
                                         selected_vertical--;
                                         return true;
                                         }
                                         if (event == Event::ArrowDown && selected_vertical < (int)vertical_entries.size() - 1) {
                                         selected_vertical++;
                                         return true;
                                         }
                                         return false;
                                         });

    // Combine all components
    Component main = Container::Vertical({
        horizontal_menu,
        vertical_menu,
    });

    auto screen = ScreenInteractive::TerminalOutput().Fullscreen();
    std::string top_text = "Library of " + this->spotify.user.display_name;
    std::string time_elapsed = "0:00";

    std::atomic<bool> running = true;
    Component renderer = CatchEvent(
        Renderer(
            main,
            [&] {
                main->TakeFocus();
                return vbox({
                    text(top_text) | center | color(Color::Green) | size(HEIGHT, EQUAL, 1) | xflex,
                    horizontal_menu->Render() | size(HEIGHT, EQUAL, 1) | xflex,
                    vertical_menu->Render() | flex,
                    text(time_elapsed)| size(HEIGHT, EQUAL, 1) | xflex,
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
                        time_elapsed = "▶️ ⏸️ " +std::to_string(player_event->data.position_changed.position_ms/1000);
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
            if (event == Event::Character('p')) {
                player_pause(this->spotify.player);
                return true;
            }
            if (event == Event::Character('P')) {
                player_play(this->spotify.player);
                return true;
            }
            if (event == Event::Character('q')) {
                running = false;
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                screen.Exit();
                return true;
            }
            return false;
        });

    std::thread([&screen, &running]() {
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            screen.PostEvent(Event::Special("tick"));
        }
    }).detach();

    screen.Loop(renderer);

}

App::~App() {}

}
