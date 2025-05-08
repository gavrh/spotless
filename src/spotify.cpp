#include <exception>
#include <iostream>
#include <librespot/oauth.h>
#include <spotify.hpp>

namespace spotify {

SpotifyItem::SpotifyItem() {}
SpotifyItem::SpotifyItem(phone::Phone &phone, std::string id) {

    phone.Perform("https://api.spotify.com/v1/tracks/" + id, true, {});
    if (phone.code == phone::SUCCESS) {

        this->id = phone.data["id"].get<std::string>();
        this->uri = phone.data["uri"].get<std::string>();
        this->name = phone.data["name"].get<std::string>();
        this->album_name = phone.data["album"]["name"].get<std::string>();
        this->artist = phone.data["artists"][0]["name"].get<std::string>();
        this->duration_ms = phone.data["duration_ms"].get<uint32_t>();
    }

}
SpotifyItem::~SpotifyItem() {}

Playlist::Playlist() {}
Playlist::~Playlist() {}

Context::Context() {}
Context::~Context() {}

User::User() {}
User::User(std::string display_name, cache::UserCache &user_cache) {
    this->display_name = display_name;
    this->access_token = user_cache.access_token;
    this->refresh_token = user_cache.refresh_token;
    this->expires_at = user_cache.expires_at;
}
User::~User() {}

void User::LikedTracks(phone::Phone &phone) {
    phone.Perform("https://api.spotify.com/v1/me/tracks?limit=50", true, {});
    const auto &items = phone.data["items"];
    for (const auto &item : items) {
        const auto &track = item["track"];
        SpotifyItem new_item;

        new_item.id = track["id"].get<std::string>();
        new_item.uri = track["uri"].get<std::string>();
        new_item.name = track["name"].get<std::string>();
        new_item.album_name = track["album"]["name"].get<std::string>();
        new_item.artist = track["artists"][0]["name"].get<std::string>();
        new_item.duration_ms = track["duration_ms"].get<uint32_t>();

        this->liked_tracks.push_back(new_item);
    }
}

void User::GetPlaylists(phone::Phone &phone) {
    phone.Perform("https://api.spotify.com/v1/me/playlists?limit=50", true, {});
    const auto items = phone.data["items"];

    for (const auto item : items) {
        Playlist playlist;

        std::string url = "https://api.spotify.com/v1/playlists/"
         + item["id"].get<std::string>()
         + "/tracks?limit=100";

        playlist.name = item["name"].get<std::string>();
        playlist.id = item["id"].get<std::string>();
        playlist.owner = item["owner"]["display_name"].get<std::string>();
        playlist.track_count = item["tracks"]["total"].get<uint32_t>();

        if (playlist.track_count == 0) continue;
        
        while (!url.empty()) {
            phone.Perform(url, true, {});
            const auto &tracks = phone.data["items"];
            for (const auto &i: tracks) {
                const auto &track = i["track"];
                SpotifyItem new_item;

                try {
                    new_item.id = track["id"].get<std::string>();
                    new_item.uri = track["uri"].get<std::string>();
                    new_item.name = track["name"].get<std::string>();
                    new_item.album_name = track["album"]["name"].get<std::string>();
                    new_item.artist = track["artists"][0]["name"].get<std::string>();
                    new_item.duration_ms = track["duration_ms"].get<uint32_t>();
                } catch (std::exception &e) {}

                playlist.tracks.push_back(new_item);
            }
            
            url = phone.data.contains("next") && !phone.data["next"].is_null() ? phone.data["next"].get<std::string>() : "";
        }

        this->playlists.push_back(playlist);
    }
}

Spotify::Spotify(cache::Cache &cache, config::Config &config) {

    cache::UserCache &user_cache = cache.GetUserCache();
    cache::PlaybackCache &playback_cache = cache.GetPlaybackCache();

    this->phone = phone::Phone();
    this->phone.SetToken(user_cache.access_token);
    this->phone.Perform("https://api.spotify.com/v1/me", true, {});

    if (user_cache.access_token.size() == 0) {
        AuthUser(cache, config);
        this->phone.Perform("https://api.spotify.com/v1/me", true, {});
    } else {
        if (this->phone.code != phone::SUCCESS) {
            // TODO: REFRESH TOKEN
            AuthUser(cache, config);
            this->phone.Perform("https://api.spotify.com/v1/me", true, {});
            std::cout << "here" << std::endl;
        }
    }

    this->user = User(this->phone.data["display_name"].get<std::string>(), user_cache);
    this->SetupContext(playback_cache);

    Credentials* creds = credentials_new(this->user.access_token.c_str());
    Session* session = session_new(session_config_default());
    session_connect(session, creds);

    this->mixer = mixer_new(mixer_config_default(), "softvol");
    this->player = player_new(player_config_default(), session, this->mixer, "pulseaudio");
    this->player_channel = player_channel_get(this->player);

}
Spotify::~Spotify() {}

void Spotify::HandleEvent(PlayerEvent* player_event) {

}

void Spotify::Preload() {}
void Spotify::Preload(std::string spotify_uri) {
    player_preload(this->player, spotify_uri.c_str());
}

void Spotify::Load(
    std::string spotify_uri,
    bool        start_playing,
    uint32_t    position_ms
) {
    player_load(this->player, spotify_uri.c_str(), start_playing, position_ms);
}

void Spotify::UpdateToken(std::string access_token) {}
void Spotify::RefreshToken() {}

void Spotify::AuthUser(cache::Cache &cache, config::Config &config) {
    OAuthBuilder* oauth = oauth_builder_new(SPOTIFY_CLIENT_ID, SPOTIFY_REDIRECT_URI);
    oauth_builder_auto_open(oauth);
    oauth_builder_add_scope(oauth, OAUTH_SCOPE_ALL);
    OAuth* auth = oauth_build(oauth);

    this->user.access_token = std::string(oauth_access_token(auth));
    this->user.refresh_token = std::string(oauth_refresh_token(auth));
    this->user.expires_at = oauth_expires_at(auth);

    if (config.cache.login) {
        cache.UpdateUserCache(
            this->user.access_token,
            this->user.refresh_token,
            this->user.expires_at
        );
    }

    this->phone.SetToken(this->user.access_token);

}

void Spotify::SetupContext(cache::PlaybackCache &playback_cache) {

    this->context.is_playing = false;
    this->context.position_ms = playback_cache.position_ms;
    this->context.volume = playback_cache.volume;
    this->context.is_track = playback_cache.is_track;
    this->context.track_id = playback_cache.track_id;
    this->context.playlist_id = playback_cache.playlist_id;

    if (!playback_cache.spotify_id.empty()) {
        this->context.current = SpotifyItem(this->phone, playback_cache.spotify_id);
    }

}

}
