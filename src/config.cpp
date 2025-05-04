#include <config.hpp>

namespace spotless {
namespace config {

ConfigOptions::ConfigOptions(const toml::node_view<toml::node> &node) {
}

ConfigTheme::ConfigTheme(const toml::node_view<toml::node> &node) {
}

Config::Config() {

    this->path = config::DEFAULT_PATH;

    toml::table config = toml::parse_file(this->path);

    const toml::node_view<toml::node> &options = config["config"];
    const toml::node_view<toml::node> &theme = config["theme"];

    this->options = ConfigOptions(options);
    this->theme = ConfigTheme(theme);

}

Config::Config(std::string path) {

    this->path = path;

}

}
}
