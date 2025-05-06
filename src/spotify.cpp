#include <librespot/oauth.h>
#include <spotify.hpp>

namespace spotify {

SpotifyItem::SpotifyItem() {}
SpotifyItem::~SpotifyItem() {}

Context::Context() {}
Context::~Context() {}

User::User() {}
User::~User() {}

Spotify::Spotify(cache::Cache &cache, config::Config &config) {

    cache::UserCache user_cache = cache.GetUserCache();

    if (user_cache.access_token.size() == 0) {

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

    } else {
        this->user.access_token = user_cache.access_token;
        this->user.refresh_token = user_cache.refresh_token;
        this->user.expires_at = user_cache.expires_at;
    }

    Credentials* creds = credentials_new(this->user.access_token.c_str());
    Session* session = session_new(session_config_default());
    session_connect(session, creds);

    this->mixer = mixer_new(mixer_config_default(), "softvol");
    this->player = player_new(player_config_default(), session, this->mixer, "pulseaudio");

    //mixer_set_volume(mixer, mixer_get_volume(this->mixer) * 0.75);

    player_load(this->player, "spotify:track:0z7ngORCg8jSVKmyF0veGT", true, 0);

    this->player_channel = player_channel_get(this->player);

}
Spotify::~Spotify() {}

void Spotify::HandleEvent(PlayerEvent* player_event) {

}

void Spotify::Preload() {}
void Spotify::Preload(std::string spotify_uri) {
    player_preload(this->player, spotify_uri.c_str());
}

void Spotify::Load() {}
void Spotify::Load(
    std::string spotify_uri,
    bool        start_playing,
    uint32_t    position_ms
) {
    player_load(this->player, spotify_uri.c_str(), start_playing, position_ms);
}

}
