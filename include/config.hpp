#ifndef SPOTLESS_CONFIG_HPP
#define SPOTLESS_CONFIG_HPP

#include <string>
#include <unordered_map>

namespace spotless {
namespace config {

class Config {
private:
    std::unordered_map<std::string, std::string> colors;

};

}
}

#endif // SPOTLESS_CONFIG_HPP
