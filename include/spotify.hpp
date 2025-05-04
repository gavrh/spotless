#ifndef SPOTLESS_SPOTIFY_HPP
#define SPOTLESS_SPOTIFY_HPP

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
    User user;
};

}
}

#endif // SPOTLESS_SPOTIFY_HPP
