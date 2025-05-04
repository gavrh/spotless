#ifndef SPOTLESS_CONFIG_HPP
#define SPOTLESS_CONFIG_HPP

#include <cstdint>
#include <string>
#include <toml++/toml.hpp>

namespace spotless {
namespace config {

// default config path
constexpr const char* DEFAULT_PATH = "~/.config/spotless/config.toml";

class ConfigOptions {

public:
    bool show_features = true;

    ConfigOptions();
    ConfigOptions(const toml::node_view<toml::node> &node);
    ~ConfigOptions();

};

class ConfigTheme {

public:
    uint32_t main =         0x000000;
    uint32_t secondary =    0xffffff;

    ConfigTheme();
    ConfigTheme(const toml::node_view<toml::node> &node);
    ~ConfigTheme();

};

class Config {

private:
    std::string     path;
    ConfigOptions   options;
    ConfigTheme     theme;

public:

    Config();
    Config(std::string path);
    ~Config();

};

}
}

#endif // SPOTLESS_CONFIG_HPP
