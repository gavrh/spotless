#ifndef SPOTLESS_SPOTIFY_HPP
#define SPOTLESS_SPOTIFY_HPP

#include <librespot/playback.h>

namespace spotless {
namespace spotify {

class User {

private:
    char* token;

public:
    

    void refresh_token();
};

class Spotify {

private:
    User    user;
    Player* player;
    Mixer*  mixer;

public:
    Spotify();
    ~Spotify();

};

}
}

#endif // SPOTLESS_SPOTIFY_HPP
