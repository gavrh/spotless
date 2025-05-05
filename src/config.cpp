#include <config.hpp>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace spotless {
namespace config {

ConfigOptions::ConfigOptions(toml::value &value) {
    this->show_featured_artists = value.contains("show_featured_artists") ? value["show_featured_artists"].as_boolean() : this->show_featured_artists;
}
ConfigOptions::~ConfigOptions() {}

ConfigCache::ConfigCache(toml::value &value) {
    this->login = value.contains("login") ? value["login"].as_boolean() : this->login;
    this->playback = value.contains("playback") ? value["playback"].as_boolean() : this->playback;
    this->songs = value.contains("songs") ? value["songs"].as_boolean() : this->songs;
}
ConfigCache::~ConfigCache() {}

ConfigTheme::ConfigTheme(toml::value &value) {
    this->main = value.contains("main") ? value["main"].as_integer() : this->main;
    this->secondary = value.contains("secondary") ? value["secondary"].as_integer() : this->secondary;
}
ConfigTheme::~ConfigTheme() {}

Config::Config() {

    this->path = this->DefaultPath();
    this->Load();

}

Config::Config(std::string path) {

    this->path = path;
    this->Load();

}

Config::~Config() {}

// TEMP
void Config::View() {
    std::cout << "= Options ="<< std::endl;
    std::cout << "Show featured: " << this->options.show_featured_artists << std::endl;
    std::cout << "= Cache ="<< std::endl;
    std::cout << "Login: " << this->cache.login << std::endl;
    std::cout << "Playback: " << this->cache.playback << std::endl;
    std::cout << "Songs: " << this->cache.songs << std::endl;
    std::cout << "= Theme ="<< std::endl;
    std::cout << "Main: " << this->theme.main << std::endl;
    std::cout << "Secondary: " << this->theme.secondary << std::endl;
}

std::string Config::DefaultPath() {

    const char* home = std::getenv("HOME");
    return std::string(home) + "/.config/spotless/config.toml";
    
}

void Config::Load() {

    if (std::filesystem::exists(this->path)) {

        toml::value config = toml::parse(this->path);

        this->options = ConfigOptions(config["options"]);
        this->cache = ConfigCache(config["cache"]);
        this->theme = ConfigTheme(config["theme"]);

    } else {

        this->path = this->DefaultPath();

        if (!std::filesystem::exists(this->path)) {

            std::filesystem::path config_file_path = this->path;
            std::filesystem::create_directory(config_file_path.parent_path());

            std::ofstream config_file(config_file_path);
            if (!config_file) {
                std::cerr << "Failed to create config file\n";
                return;
            }
        }

        this->Load();
    }

}

}
}
