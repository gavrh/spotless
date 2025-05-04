#include <cache.hpp>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>

namespace spotless {
namespace cache {

UserCache::UserCache(toml::value &value) {
    this->access_token = value.contains("access_token") ? value["access_token"].as_string() : this->access_token;
    this->refresh_token = value.contains("refresh_token") ? value["refresh_token"].as_string() : this->refresh_token;
}
UserCache::~UserCache() {}

PlaybackCache::PlaybackCache(toml::value &value) {
    this->spotify_id = value.contains("spotify_id") ? value["spotify_id"].as_string() : this->spotify_id;

    this->position_ms = value.contains("position_ms") ? value["position_ms"].as_integer() : this->position_ms;
    
    this->volume = value.contains("volume") ? value["volume"].as_integer() : this->volume;
}
PlaybackCache::~PlaybackCache() {}

Cache::Cache() {
    std::string path = this->CachePath();

    if (std::filesystem::exists(path)) {

        toml::value cache = toml::parse(path);

        this->user = UserCache(cache["user"]);
        this->playback = PlaybackCache(cache["playback"]);

    } else {


        std::filesystem::path cache_file_path = path;
        std::filesystem::create_directory(cache_file_path.parent_path());

        std::ofstream config_file(cache_file_path);
        if (!config_file) {
            std::cerr << "Failed to create cache file\n";
        } else {

            toml::value cache;

            cache["user"]["access_token"] = this->user.access_token;
            cache["user"]["refresh_token"] = this->user.refresh_token;

            cache["playback"]["spotify_id"] = this->playback.spotify_id;
            cache["playback"]["position_ms"] = this->playback.position_ms;
            cache["playback"]["volume"] = this->playback.volume;

            std::ofstream cache_file(path);
            cache_file << std::setw(80) << cache;
            cache_file.close();

        }

    }

}
Cache::~Cache() {}

UserCache& Cache::GetUserCache() {
    return this->user;
}

PlaybackCache& Cache::GetPlaybackCache() {
    return this->playback;
}

void Cache::UpdateUserCache(
    std::string access_token,
    std::string refresh_token
) {

    this->user.access_token = access_token;
    this->user.refresh_token = refresh_token;

    std::string path = this->CachePath();

    if (std::filesystem::exists(path)) {

        toml::value cache = toml::parse(path);
        cache["user"]["access_token"] = this->user.access_token;
        cache["user"]["refresh_token"] = this->user.refresh_token;

        std::ofstream cache_file(path);
        cache_file << std::setw(80) << cache;
        cache_file.close();

    }
}

void Cache::UpdatePlaybackCache(
    std::string spotify_id,
    uint32_t    position_ms,
    uint32_t    volume
) {
    this->playback.spotify_id = spotify_id;
    this->playback.position_ms = position_ms;
    this->playback.volume = volume;

    std::string path = this->CachePath();

    if (std::filesystem::exists(path)) {

        toml::value cache = toml::parse(path);
        cache["playback"]["spotify_id"] = this->playback.spotify_id;
        cache["playback"]["position_ms"] = this->playback.position_ms;
        cache["playback"]["volume"] = this->playback.volume;

        std::ofstream cache_file(path);
        cache_file << std::setw(80) << cache;
        cache_file.close();

    }
}

std::string Cache::CachePath() {

    const char* home = std::getenv("HOME");
    return std::string(home) + "/.config/spotless/cache.toml";

}

}
}
