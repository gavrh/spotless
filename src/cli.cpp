#include <cache.hpp>
#include <cli.hpp>
#include <config.hpp>
#include <CLI/CLI.hpp>
#include <curl/curl.h>
#include <spotify.hpp>
#include <ui.hpp>

namespace cli {

void handle(int argc, char** argv) {

    curl_global_init(CURL_GLOBAL_ALL);

    cache::Cache cache;
    config::Config config;
    spotify::Spotify spotify(cache, config);
    ui::App app(cache, config, spotify);

    curl_global_cleanup();

}

}
