#ifndef SPOTLESS_CACHE_HPP
#define SPOTLESS_CACHE_HPP

#include <string>

namespace spotless {
namespace cache {

class Cache {

private:

    Cache();
    Cache(std::string path);
    ~Cache();

};

}
}


#endif // SPOTLESS_CACHE_HPP
