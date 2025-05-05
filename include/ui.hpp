#ifndef SPOTLESS_UI_HPP
#define SPOTLESS_UI_HPP

#include <cache.hpp>
#include <config.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <spotify.hpp>
#include <vector>

namespace spotless {
namespace ui {


class App {

private:
    cache::Cache                    &cache;
    config::Config                  &config;
    spotify::Spotify                &spotify;
    std::vector<ftxui::Component>   views;
    
public:
    
    App(
        cache::Cache        &cache,
        config::Config      &config,
        spotify::Spotify    &spotify
    );
    ~App();

};

}
}

#endif // SPOTLESS_UI_HPP
