#ifndef SPOTLESS_CONFIG_HPP
#define SPOTLESS_CONFIG_HPP

#include <cstdint>
#include <string>
#include <toml++/toml.hpp>

namespace spotless {
namespace config {

class ConfigOptions {

public:
    bool show_features = true;

    ConfigOptions() = default;
    ConfigOptions(toml::node_view<toml::node> node);
    ~ConfigOptions();

};

class ConfigTheme {

public:
    uint32_t main =         0x000000;
    uint32_t secondary =    0xffffff;

    ConfigTheme() = default;
    ConfigTheme(toml::node_view<toml::node> node);
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

    // TEMP
    void View();

private:

    std::string DefaultPath();
    void Load();

};

}
}

#endif // SPOTLESS_CONFIG_HPP
