#include <config.hpp>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace spotless {
namespace config {

ConfigOptions::ConfigOptions(toml::node_view<toml::node> node) {
    this->cache_login = node["cache_login"].value_or(this->cache_login);
    this->cache_songs = node["cache_songs"].value_or(this->cache_songs);
    this->show_featured_artists = node["show_featured_artists"].value_or(this->show_featured_artists);
}
ConfigOptions::~ConfigOptions() {
}

ConfigTheme::ConfigTheme(toml::node_view<toml::node> node) {
    this->main = node["main"].value_or(this->main);
    this->secondary = node["secondary"].value_or(this->secondary);
}
ConfigTheme::~ConfigTheme() {
}

Config::Config() {

    this->path = this->DefaultPath();
    this->Load();

}

Config::Config(std::string path) {

    this->path = path;
    this->Load();

}

Config::~Config() {
}

// TEMP
void Config::View() {
    std::cout << "Options"<< std::endl;
    std::cout << "Cache login: " << this->options.cache_login << std::endl;
    std::cout << "Cache songs: " << this->options.cache_songs << std::endl;
    std::cout << "Show featured: " << this->options.show_featured_artists << std::endl;
    std::cout << "Theme"<< std::endl;
    std::cout << "Main: " << this->theme.main << std::endl;
    std::cout << "Secondary: " << this->theme.secondary << std::endl;
}

std::string Config::DefaultPath() {

    const char* home = std::getenv("HOME");
    return std::string(home) + "/.config/spotless/config.toml";
    
}

void Config::Load() {

    if (std::filesystem::exists(this->path)) {

        toml::table config = toml::parse_file(this->path);

        this->options = ConfigOptions(config["config"]);
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
