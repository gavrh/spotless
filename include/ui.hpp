#ifndef SPOTLESS_UI_HPP
#define SPOTLESS_UI_HPP

#include <cache.hpp>
#include <config.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <spotify.hpp>

namespace ui {

class CommandLine {

};

class PlaylistView {

};

class App {

private:
    cache::Cache                    &cache;
    config::Config                  &config;
    spotify::Spotify                &spotify;
    
public:
    
    App(
        cache::Cache        &cache,
        config::Config      &config,
        spotify::Spotify    &spotify
    );
    ~App();

private:
    void UpdateUserCache();
    void UpdatePlaybackCache();

    ftxui::Component List(
        ftxui::ScreenInteractive &screen,
        int &horizontal_selected,
        int &tracks_selected,
        int &playlists_selected,
        bool &in_playlist,
        int &in_playlist_selected,
        int &tracks_offset,
        int &playlists_offset,
        int &in_playlist_offset
    );
    ftxui::Component TimeBar(ftxui::ScreenInteractive &screen);
    ftxui::Component CurrentBar();
    bool HandleEvent(ftxui::Event &event);

};

}

#endif // SPOTLESS_UI_HPP
