#include <librespot/oauth.h>
#include <spotify.hpp>

namespace spotless {
namespace spotify {

SpotifyItem::SpotifyItem() {}
SpotifyItem::~SpotifyItem() {}

Context::Context() {}
Context::~Context() {}

User::User() {}
User::~User() {}

Spotify::Spotify(cache::Cache &cache) {
    OAuthBuilder* oauth = oauth_builder_new(SPOTIFY_CLIENT_ID, SPOTIFY_REDIRECT_URI);
    oauth_builder_auto_open(oauth);
    oauth_builder_add_scope(oauth, OAUTH_SCOPE_ALL);
    OAuth* auth = oauth_build(oauth);

    const char* access = oauth_access_token(auth);
    Credentials* creds = credentials_new(access);
    Session* session = session_new(session_config_default());
    session_connect(session, creds);

    this->mixer = mixer_new(mixer_config_default(), "softvol");
    this->player = player_new(player_config_default(), session, this->mixer, "pulseaudio");

    mixer_set_volume(mixer, mixer_get_volume(this->mixer) * 0.75);
    player_load(this->player, "spotify:track:55rXApL8KBessVIpJpH4Fw", true, 45000);

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
}
