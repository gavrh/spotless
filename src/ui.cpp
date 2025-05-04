#include <ui.hpp>

namespace spotless {
namespace ui {

App::App(
    cache::Cache        &cache,
    config::Config      &config,
    spotify::Spotify    &spotify
) : cache(cache), config(config), spotify(spotify) {}

App::~App() {}

}
}
