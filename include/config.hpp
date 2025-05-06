#ifndef SPOTLESS_CONFIG_HPP
#define SPOTLESS_CONFIG_HPP

#include <cstdint>
#include <string>
#include <toml.hpp>

namespace config {

class ConfigOptions {

public:
    bool show_featured_artists =    true;

    ConfigOptions() = default;
    ConfigOptions(toml::value &value);
    ~ConfigOptions();

};

class ConfigCache {

public:
    bool login = true;
    bool playback = true;
    bool songs = false;

    ConfigCache() = default;
    ConfigCache(toml::value &value);
    ~ConfigCache();

};

class ConfigTheme {

public:
    uint32_t main =         0x000000;
    uint32_t secondary =    0xffffff;

    ConfigTheme() = default;
    ConfigTheme(toml::value &value);
    ~ConfigTheme();

};

class Config {

public:
    std::string     path;
    ConfigOptions   options;
    ConfigCache     cache;
    ConfigTheme     theme;

    Config();
    Config(std::string path);
    ~Config();

    // TEMP
    void View();

private:

    std::string DefaultPath();
    void CreateDefault();
    void Load();

};

}

#endif // SPOTLESS_CONFIG_HPP
