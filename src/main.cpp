#include "config.hpp"
#include <cli.hpp>

int main(int argc, char** argv) {

    spotless::config::Config conf;
    spotless::cli::handle(argc, argv);
    
    conf.View();

    return 0;
}
