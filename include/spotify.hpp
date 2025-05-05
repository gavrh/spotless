#ifndef SPOTLESS_SPOTIFY_HPP
#define SPOTLESS_SPOTIFY_HPP

#include <cache.hpp>
#include <cstdint>
#include <librespot/playback.h>
#include <queue>
#include <string>
#include <vector>

namespace spotless {
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
    SpotifyItemType             type;
    std::string                 artist;
    std::vector<std::string>    features;

    SpotifyItem();
    ~SpotifyItem();

};

class Context {

public:
    SpotifyItem             current;
    uint32_t                position_ms;
    std::queue<SpotifyItem> queue;

    Context();
    ~Context();

};

class User {

public:
    std::string access_token;
    std::string refresh_token;
    uint32_t    expires_at;

    User();
    ~User();


};

class Spotify {

private:
    Context         context;
    User            user;
    Player*         player;
    PlayerChannel*  player_channel;
    Mixer*          mixer;

public:
    Spotify(cache::Cache &cache);
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

};

}
}

#endif // SPOTLESS_SPOTIFY_HPP
