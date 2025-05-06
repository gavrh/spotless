#include "ui.hpp"
#include <cache.hpp>
#include <cli.hpp>
#include <config.hpp>
#include <CLI/CLI.hpp>
#include <iostream>
#include <spotify.hpp>

namespace cli {

void handle(int argc, char** argv) {
    std::cout << argc << std::endl;

    cache::Cache cache;
    config::Config config;
    spotify::Spotify spotify(cache, config);
    ui::App(cache, config, spotify);

}

}
