#include <cache.hpp>
#include <cli.hpp>
#include <config.hpp>
#include <iostream>
#include <CLI/CLI.hpp>

namespace spotless {
namespace cli {

void handle(int argc, char** argv) {
    std::cout << argc << std::endl;

    cache::Cache cache;

    config::Config config;
    config.View();

}

}
}
