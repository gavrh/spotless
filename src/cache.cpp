#include <cache.hpp>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>

namespace cache {

UserCache::UserCache(toml::value &value) {
    this->access_token = value.contains("access_token") ? std::string(value["access_token"].as_string()) : this->access_token;
    this->refresh_token = value.contains("refresh_token") ? std::string(value["refresh_token"].as_string()) : this->refresh_token;
    this->expires_at = value.contains("expires_at") ? value["expires_at"].as_integer() : this->expires_at;
}
UserCache::~UserCache() {}

PlaybackCache::PlaybackCache(toml::value &value) {
    this->spotify_id = value.contains("spotify_id") ? std::string(value["spotify_id"].as_string()) : this->spotify_id;
    this->position_ms = value.contains("position_ms") ? value["position_ms"].as_integer() : this->position_ms;
    this->volume = value.contains("volume") ? value["volume"].as_integer() : this->volume;
    this->is_track = value.contains("is_track") ? value["is_track"].as_boolean() : this->is_track;
    this->track_id = value.contains("track_id") ? std::string(value["track_id"].as_string()) : this->track_id;
    this->playlist_id = value.contains("playlist_id") ? std::string(value["playlist_id"].as_string()) : this->playlist_id;
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
            cache["user"]["expires_at"] = this->user.expires_at;

            cache["playback"]["spotify_id"] = this->playback.spotify_id;
            cache["playback"]["position_ms"] = this->playback.position_ms;
            cache["playback"]["volume"] = this->playback.volume;
            cache["playback"]["is_track"] = this->playback.is_track;
            cache["playback"]["track_id"] = this->playback.track_id;
            cache["playback"]["playlist_id"] = this->playback.playlist_id;

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
    std::string refresh_token,
    uint64_t    expires_at
) {

    this->user.access_token = access_token;
    this->user.refresh_token = refresh_token;
    this->user.expires_at = expires_at;

    std::string path = this->CachePath();

    if (std::filesystem::exists(path)) {

        toml::value cache = toml::parse(path);
        cache["user"]["access_token"] = this->user.access_token;
        cache["user"]["refresh_token"] = this->user.refresh_token;
        cache["user"]["expires_at"] = this->user.expires_at;

        std::ofstream cache_file(path);
        cache_file << std::setw(80) << cache;
        cache_file.close();

    }
}

void Cache::UpdatePlaybackCache(
    std::string spotify_id,
    uint32_t    position_ms,
    uint32_t    volume,
    bool        is_track,
    std::string track_id,
    std::string playlist_id
) {
    this->playback.spotify_id = spotify_id;
    this->playback.position_ms = position_ms;
    this->playback.volume = volume;
    this->playback.is_track = is_track;
    this->playback.track_id = track_id;
    this->playback.playlist_id = playlist_id;

    std::string path = this->CachePath();

    if (std::filesystem::exists(path)) {

        toml::value cache = toml::parse(path);
        cache["playback"]["spotify_id"] = this->playback.spotify_id;
        cache["playback"]["position_ms"] = this->playback.position_ms;
        cache["playback"]["volume"] = this->playback.volume;
        cache["playback"]["is_track"] = this->playback.is_track;
        cache["playback"]["track_id"] = this->playback.track_id;
        cache["playback"]["playlist_id"] = this->playback.playlist_id;

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
