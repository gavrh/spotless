#ifndef SPOTLESS_CACHE_HPP
#define SPOTLESS_CACHE_HPP

#include <cstdint>
#include <string>
#include <toml++/toml.hpp>

namespace spotless {
namespace cache {

class UserCache {

public:
    std::string access_token =  "";
    std::string refresh_token = "";

    UserCache() = default;
    UserCache(toml::node_view<toml::node> node);
    ~UserCache();

};

class PlaybackCache {
    
public:
    std::string spotify_id =    "";
    uint32_t    position_ms =   0;

    PlaybackCache() = default;
    PlaybackCache(toml::node_view<toml::node> node);
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

private:
    std::string CachePath();

};

}
}


#endif // SPOTLESS_CACHE_HPP
