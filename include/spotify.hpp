#ifndef SPOTLESS_SPOTIFY_HPP
#define SPOTLESS_SPOTIFY_HPP

#include "phone.hpp"
#include <cache.hpp>
#include <config.hpp>
#include <cstdint>
#include <curl/curl.h>
#include <ftxui/component/component_base.hpp>
#include <librespot/playback.h>
#include <queue>
#include <string>
#include <vector>

namespace spotify {

static constexpr const char* SPOTIFY_CLIENT_ID = "65b708073fc0480ea92a077233ca87bd";
static constexpr const char* SPOTIFY_REDIRECT_URI = "http://127.0.0.1:8989/login";

enum SpotifyItemType {
    TRACK,
    EPISODE
};

class SpotifyItem {

public:
    std::string                 id;
    std::string                 uri;
    std::string                 name;
    SpotifyItemType             type;
    std::string                 artist;
    std::vector<std::string>    features;
    uint32_t                    duration_ms;

    SpotifyItem();
    SpotifyItem(phone::Phone &phone, std::string id);
    ~SpotifyItem();

    ftxui::Component Render(); 

};

class Context {

public:
    SpotifyItem             current;
    bool                    is_playing;
    uint32_t                position_ms;
    uint32_t                volume;
    std::queue<SpotifyItem> queue;

    Context();
    ~Context();

};

class User {

public:
    std::string display_name;
    std::string access_token;
    std::string refresh_token;
    uint64_t    expires_at;

    User();
    User(std::string display_name, cache::UserCache &user_cache);
    ~User();


};

class Spotify {

public:
    Context         context;
    User            user;
    Player*         player;
    PlayerChannel*  player_channel;
    Mixer*          mixer;
    phone::Phone    phone;

    Spotify(cache::Cache &cache, config::Config &config);
    ~Spotify();

    void HandleEvent(PlayerEvent* player_event);
    void Preload();
    void Preload(std::string spotify_uri);
    void Load();
    void Load(
        std::string spotify_uri,
        bool        start_playing,
        uint32_t    position_ms
    );
    uint32_t PositionMs();

    void UpdateToken(std::string access_token);
    void RefreshToken();

private:
    void AuthUser(cache::Cache &cache, config::Config &config);
    void SetupContext(cache::PlaybackCache &playback_cache);

};

}

#endif // SPOTLESS_SPOTIFY_HPP
