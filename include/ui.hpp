#ifndef SPOTLESS_UI_HPP
#define SPOTLESS_UI_HPP

#include <cache.hpp>
#include <config.hpp>
#include <spotify.hpp>

namespace spotless {
namespace ui {

class App {

private:
    cache::Cache &cache;
    config::Config &config;
    spotify::Spotify &spotify;
    
public:
    
    App(
        cache::Cache &cache,
        config::Config &config,
        spotify::Spotify &spotify
    );

    ~App();

};

}
}

#endif // SPOTLESS_UI_HPP
