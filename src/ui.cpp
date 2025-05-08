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

    if (!this->cache.GetPlaybackCache().spotify_id.empty()) this->spotify.Load(this->spotify.context.current.uri, false, spotify.context.position_ms);
    mixer_set_volume(this->spotify.mixer, UINT16_MAX * (0.01 * this->spotify.context.volume));

    auto screen = ScreenInteractive::TerminalOutput().Fullscreen();

    // press delay
    auto last_press_time = std::chrono::steady_clock::now();
    const auto delay = std::chrono::milliseconds(250);

    // selections
    std::vector<std::string> horizontal_entries = {"Tracks", "Playlists", "Help"};
    int selected_horizontal = 0;
    int tracks_selected = 0;
    int playlists_selected = 0;
    bool in_playlist = false;
    int in_playlist_selected = 0;

    Component horizontal_menu = Renderer([&] {
        Elements items;
        for (size_t i = 0; i < horizontal_entries.size(); ++i) {
            bool selected = (int)i == selected_horizontal;
            items.push_back(
                center(text(horizontal_entries[i])) |
                (selected ? bgcolor(Color::Cyan) | color(Color::White) : nothing) |
                flex
            );
        }
        return hbox(std::move(items)) | xflex;
    });


    Component main = Container::Vertical({
        horizontal_menu,
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
                    (!in_playlist ? (horizontal_menu->Render() | size(HEIGHT, EQUAL, 1) | xflex)
                    : (dbox({text("< Back to Playlists") | color(Color::Green), text(" " + this->spotify.user.playlists[playlists_selected].name + " ") | color(Color::White) | bgcolor(Color::Cyan) | center }))),
                    this->List(
                        screen,
                        selected_horizontal,
                        tracks_selected,
                        playlists_selected,
                        in_playlist,
                        in_playlist_selected
                    )->Render() | yflex,
                    this->TimeBar(screen)->Render(),
                    this->CurrentBar()->Render(), 
                }) | flex;
            }), [&](Event event) {
            if (event == Event::Special("tick")) {
                PlayerEvent* player_event;
                if (player_channel_poll(this->spotify.player_channel, player_event)) {
                    if (player_event->event == PLAYER_EVENT_TIME_TO_PRELOAD_NEXT_TRACK) {
                        this->spotify.Preload("spotify:track:02JIdsrod3BYucThfUFDUX");
                    } else if (player_event->event == PLAYER_EVENT_END_OF_TRACK) {
                        this->spotify.Load("spotify:track:02JIdsrod3BYucThfUFDUX", true, 0);
                        player_pause(this->spotify.player);
                        this->spotify.context.is_playing = false;
                    } else if (player_event->event == PLAYER_EVENT_POSITION_CHANGED) {
                        spotify.context.position_ms = player_event->data.position_changed.position_ms;
                        this->UpdatePlaybackCache();
                    }
                }
                return true;
            }
            if (event == Event::Character('j') && selected_horizontal != 2) {
                int &selected_vertical = selected_horizontal == 0 ? tracks_selected : in_playlist ? in_playlist_selected : playlists_selected;
                selected_vertical = selected_vertical == (int)(selected_horizontal == 0 ? this->spotify.user.liked_tracks.size() : in_playlist ? this->spotify.user.playlists[playlists_selected].tracks.size() : this->spotify.user.playlists.size())-1 ? selected_vertical : ++selected_vertical;
                return true;
            }
            if (event == Event::Character('k') && selected_horizontal != 2) {
                int &selected_vertical = selected_horizontal == 0 ? tracks_selected : in_playlist ? in_playlist_selected : playlists_selected;
                selected_vertical = selected_vertical == 0 ? selected_vertical : --selected_vertical;
                return true;
            }
            if (event == Event::Character('l') && !in_playlist) {
                selected_horizontal = selected_horizontal == horizontal_entries.size()-1 ? selected_horizontal : ++selected_horizontal;
                return true;
            }
            if (event == Event::Character('h') && !in_playlist) {
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
                this->UpdatePlaybackCache();
                return true;
            }
            if (event == Event::Character('b')) {
                auto now = std::chrono::steady_clock::now();
                if (now - last_press_time > delay) {
                    last_press_time = now;
                    player_seek(spotify.player, spotify.context.position_ms < 1000 ? 0 : spotify.context.position_ms - 1000);
                }
                this->UpdatePlaybackCache();
                return true;
            }
            if (event == Event::Character('F')) {
                auto now = std::chrono::steady_clock::now();
                if (now - last_press_time > delay) {
                    last_press_time = now;
                    player_seek(spotify.player, spotify.context.position_ms + 5000);
                }
                this->UpdatePlaybackCache();
                return true;
            }
            if (event == Event::Character('B')) {
                auto now = std::chrono::steady_clock::now();
                if (now - last_press_time > delay) {
                    last_press_time = now;
                    player_seek(spotify.player, spotify.context.position_ms < 5000 ? 0 : spotify.context.position_ms - 5000);
                }
                this->UpdatePlaybackCache();
                return true;
            }
            if (event == Event::Character('+')) {
                spotify.context.volume++;
                if (spotify.context.volume > 100) spotify.context.volume = 100;
                mixer_set_volume(this->spotify.mixer, UINT16_MAX * (0.01 * this->spotify.context.volume));
                this->UpdatePlaybackCache();
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
                this->UpdatePlaybackCache();
                return true;
            }
            if (event == Event::Character(']')) {
                spotify.context.volume+=5;
                if (spotify.context.volume > 100) spotify.context.volume = 100;
                mixer_set_volume(this->spotify.mixer, UINT16_MAX * (0.01 * this->spotify.context.volume));
                this->UpdatePlaybackCache();
                return true;
            }
            if (event == Event::Character('[')) {
                if (spotify.context.volume < 5) {
                    spotify.context.volume = 0;
                } else {
                    spotify.context.volume-=5;
                }
                mixer_set_volume(this->spotify.mixer, UINT16_MAX * (0.01 * this->spotify.context.volume));
                this->UpdatePlaybackCache();
                return true;
            }
            if (event == Event::Character('q') || event == Event::CtrlC) {
                running = false;
                this->UpdatePlaybackCache();
                screen.Exit();
                return true;
            }
            if (event == Event::Return && selected_horizontal != 2) {
                if (selected_horizontal == 0) {
                    this->spotify.Load(this->spotify.user.liked_tracks[tracks_selected].uri, true, 0);
                    this->spotify.context.current = this->spotify.user.liked_tracks[tracks_selected];
                    this->spotify.context.is_playing = true;
                    this->spotify.context.is_track = true;
                    this->spotify.context.track_id = this->spotify.user.liked_tracks[tracks_selected].id;
                } else {
                    this->spotify.Load(this->spotify.user.playlists[playlists_selected].tracks[in_playlist_selected].uri, true, 0);
                    this->spotify.context.current = this->spotify.user.playlists[playlists_selected].tracks[in_playlist_selected];
                    this->spotify.context.is_playing = true;
                    this->spotify.context.position_ms = 0;
                    this->spotify.context.is_track = false;
                    this->spotify.context.track_id = this->spotify.user.playlists[playlists_selected].tracks[in_playlist_selected].id;
                    this->spotify.context.playlist_id  = this->spotify.user.playlists[playlists_selected].id;
                }
            }
            if (event == Event::Character('o') && selected_horizontal == 1 && !in_playlist) {
                in_playlist = true;
                in_playlist_selected = 0;
            }
            if (event == Event::Backspace && in_playlist) {
                in_playlist = false;
                in_playlist_selected = 0;
            }
            return false;
        });

    std::thread([&screen, &running]() {
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            screen.PostEvent(Event::Special("tick"));
        }
    }).detach();

    spotify::User &user = this->spotify.user;
    std::thread([&user]() {
        phone::Phone phone;
        phone.SetToken(user.access_token);
        user.LikedTracks(phone);
    }).detach();
    std::thread([&user]() {
        phone::Phone phone;
        phone.SetToken(user.access_token);
        user.GetPlaylists(phone);
    }).detach();

    screen.Loop(renderer);

}

App::~App() {}

void App::UpdateUserCache() {

}

void App::UpdatePlaybackCache() {
    this->cache.UpdatePlaybackCache(
        this->spotify.context.current.id,
        this->spotify.context.position_ms,
        this->spotify.context.volume,
        this->spotify.context.is_track,
        this->spotify.context.track_id,
        this->spotify.context.playlist_id
    );
}

ftxui::Component App::List(
    ftxui::ScreenInteractive &screen,
    int &horizontal_selected,
    int &tracks_selected,
    int &playlists_selected,
    bool &in_playlist,
    int &in_playlist_selected
) {
    return Renderer([&]() {

        if (horizontal_selected != 2) {
            int selected_vertical;
            bool is_tracks;

            if (horizontal_selected == 0) {
                selected_vertical = tracks_selected;
                is_tracks = true;
            } else {
                if (in_playlist) {
                    selected_vertical = in_playlist_selected;
                } else {
                    selected_vertical = playlists_selected;
                }
                is_tracks = false;
            }

            int scroll_offset = 0;
            int visible_lines = screen.dimy()-4;
            if (selected_vertical < scroll_offset)
                scroll_offset = selected_vertical;
            else if (selected_vertical >= scroll_offset + visible_lines)
                scroll_offset = selected_vertical - visible_lines + 1;

            Elements items;
            for (int i = scroll_offset; i < scroll_offset + visible_lines && i < (is_tracks ? this->spotify.user.liked_tracks.size() : in_playlist ? this->spotify.user.playlists[playlists_selected].tracks.size() : this->spotify.user.playlists.size()); ++i) {
                bool is_selected = i == selected_vertical;
                if (is_tracks) {
                    int duration = this->spotify.user.liked_tracks[i].duration_ms/1000;
                    std::ostringstream dur;
                    dur << (duration / 60) << ":" << std::setw(2) << std::setfill('0') << (duration % 60);
                    bool is_current = false;
                    if (
                        this->spotify.context.is_track &&
                        this->spotify.context.track_id == this->spotify.user.liked_tracks[i].id
                    ) is_current = true;
                    auto item = this->spotify.user.liked_tracks.size() != 0 ? 
                        hbox({
                            text(this->spotify.user.liked_tracks[i].artist + " - " + this->spotify.user.liked_tracks[i].name) | xflex | (is_current ? color(Color::BlueLight) : nothing),
                            text(
                                dur.str()
                            ) | (is_current ? color(Color::BlueLight) : nothing)
                        }) | (is_selected ? bgcolor(Color::Green) | (is_current ? color(Color::BlueLight) : color(Color::White)) : nothing)
                        : text("");
                    items.push_back(item);
                } else {
                    if (in_playlist) {
                        bool is_current = false;
                        if (
                            !this->spotify.context.is_track &&
                            this->spotify.context.track_id == this->spotify.user.playlists[playlists_selected].tracks[i].id
                        ) is_current = true;
                        int duration = this->spotify.user.playlists[playlists_selected].tracks[i].duration_ms/1000;
                        std::ostringstream dur;
                        dur << (duration / 60) << ":" << std::setw(2) << std::setfill('0') << (duration % 60);
                        auto item = this->spotify.user.playlists[playlists_selected].tracks.size() != 0 ? 
                            hbox({
                                text(this->spotify.user.playlists[playlists_selected].tracks[i].artist + " - " + this->spotify.user.playlists[playlists_selected].tracks[i].name) | xflex | (is_current ? color(Color::BlueLight) : nothing),
                                text(
                                    dur.str()
                                ) | (is_current ? color(Color::BlueLight) : nothing)
                            }) | (is_selected ? bgcolor(Color::Green) | color(Color::White) : nothing)
                            : text("");
                        items.push_back(item);
                    } else {
                        bool is_current = false;
                        if (
                            !this->spotify.context.is_track &&
                            this->spotify.context.playlist_id == this->spotify.user.playlists[i].id
                        ) is_current = true;
                        auto item = this->spotify.user.playlists.size() != 0 ? 
                            hbox({
                                text(this->spotify.user.playlists[i].name + " - " + this->spotify.user.playlists[i].owner) | (is_current ? color(Color::BlueLight) : nothing),
                                text(" ") | xflex,
                                text(
                                    std::to_string(this->spotify.user.playlists[i].track_count)
                                    + " tracks"
                                ) | (is_current ? color(Color::BlueLight) : nothing)
                            }) | (is_selected ? bgcolor(Color::Green) | color(Color::White) : nothing)
                            : text("");
                        items.push_back(item);
                    }
                }
            }

            return vbox(std::move(items));
        } else {
            return vbox({
                text(""),
                text("Keybindings") | bold,
                text(""),
                hbox({text("j") | bold, text(" -> down")}),
                hbox({text("k") | bold, text(" -> up")}),
                hbox({text("h") | bold, text(" -> right")}),
                hbox({text("l") | bold, text(" -> left")}),
                hbox({text("P") | bold, text(" -> toggle play")}),
                hbox({text("f") | bold, text(" -> seek forward 1s")}),
                hbox({text("b") | bold, text(" -> seek backward 1s")}),
                hbox({text("F") | bold, text(" -> seek forward 5s")}),
                hbox({text("B") | bold, text(" -> seek backward 5s")}),
                hbox({text("+") | bold, text(" -> volume up 1%")}),
                hbox({text("-") | bold, text(" -> volume down 1%")}),
                hbox({text("]") | bold, text(" -> volume up 5%")}),
                hbox({text("[") | bold, text(" -> volume down 5%")}),
                hbox({text("o") | bold, text(" -> open playlist")}),
                hbox({text("backspace") | bold, text(" -> close playlist")}),
                hbox({text("enter") | bold, text(" -> play selected")}),
                hbox({text("q") | bold, text(" -> quit")}),
            });
        }

    });
}

ftxui::Component App::TimeBar(ftxui::ScreenInteractive &screen) {
    return Renderer([&]() {
        float ratio = static_cast<float>(spotify.context.position_ms) / spotify.context.current.duration_ms;
        int total_width = screen.dimx();

        int filled_len = static_cast<int>(ratio * total_width);
        int empty_len = total_width - filled_len;

        std::string bar =
            repeat_utf8("━", filled_len) +
            repeat_utf8("╌", empty_len);


        return text(bar) | color(Color::Cyan);
    });
}

ftxui::Component App::CurrentBar() {
    return Renderer([&]() {
        int time = spotify.context.position_ms/1000;
        int time_full = spotify.context.current.duration_ms/1000;

        std::ostringstream t1,t2;
        t1 << (time / 60) << ":" << std::setw(2) << std::setfill('0') << (time % 60);
        t2 << (time_full / 60) << ":" << std::setw(2) << std::setfill('0') << (time_full % 60);

        return hbox({
            text(
                (spotify.context.is_playing
                ? "⏸️" : "▶️")
                + spotify.context.current.artist
                + " - " + spotify.context.current.name
            ),
            text("  ") | xflex,
            text(
                t1.str()
                + " / " + t2.str()
                + " [" + std::to_string(spotify.context.volume)
                + "%%]"
            )
        });
    });
}

bool App::HandleEvent(ftxui::Event &event) {
    return false;
};

}
