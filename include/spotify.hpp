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

}
}

#endif // SPOTLESS_SPOTIFY_HPP
