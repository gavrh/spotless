#include <cache.hpp>
#include <filesystem>
#include <iostream>

namespace spotless {
namespace cache {

UserCache::UserCache(toml::node_view<toml::node> node) {
   this->access_token = node["access_token"].value_or(this->access_token);
   this->refresh_token = node["refresh_token"].value_or(this->refresh_token);
}
UserCache::~UserCache() {}

PlaybackCache::PlaybackCache(toml::node_view<toml::node> node) {
   this->spotify_id = node["spotify_id"].value_or(this->spotify_id);
   this->position_ms = node["position_ms"].value_or(this->position_ms);
}
PlaybackCache::~PlaybackCache() {}

Cache::Cache() {
    std::string path = this->CachePath();

    if (std::filesystem::exists(path)) {

        toml::table config = toml::parse_file(path);

        this->user = UserCache(config["user"]);
        this->playback = PlaybackCache(config["playback"]);

    } else {

        if (!std::filesystem::exists(path)) {

            std::filesystem::path config_file_path = path;
            std::filesystem::create_directory(config_file_path.parent_path());

            std::ofstream config_file(config_file_path);
            if (!config_file) {
                std::cerr << "Failed to create cache file\n";
            }
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

std::string Cache::CachePath() {

    const char* home = std::getenv("HOME");
    return std::string(home) + "/.config/spotless/cache.toml";

}

}
}
