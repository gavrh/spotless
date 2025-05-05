#include <cache.hpp>
#include <cli.hpp>
#include <config.hpp>
#include <CLI/CLI.hpp>
#include <iostream>
#include <spotify.hpp>

namespace spotless {
namespace cli {

void handle(int argc, char** argv) {
    std::cout << argc << std::endl;

    cache::Cache cache;

    config::Config config;
    config.View();

    spotify::Spotify spotify(cache);

    while (true) {}

}

}
}
