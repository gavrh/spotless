#ifndef SPOTLESS_CACHE_HPP
#define SPOTLESS_CACHE_HPP

#include <cstdint>
#include <string>
#include <toml.hpp>

namespace cache {

class UserCache {

public:
    std::string access_token =  "";
    std::string refresh_token = "";
    uint64_t expires_at =       0;

    UserCache() = default;
    UserCache(toml::value &value);
    ~UserCache();

};

class PlaybackCache {

public:
    std::string spotify_id =    "";
    uint32_t    position_ms =   0;
    uint32_t    volume =        50;

    PlaybackCache() = default;
    PlaybackCache(toml::value &value);
    ~PlaybackCache();
};

class Cache {

private:
    UserCache       user;
    PlaybackCache   playback;

public:
    Cache();
    ~Cache();

    UserCache& GetUserCache();
    PlaybackCache& GetPlaybackCache();

    void UpdateUserCache(
        std::string access_token,
        std::string refresh_token,
        uint64_t    expires_at
    );
    void UpdatePlaybackCache(
        std::string spotify_id,
        uint32_t    position_ms,
        uint32_t    volume
    );

private:
    std::string CachePath();

};

}

#endif // SPOTLESS_CACHE_HPP
