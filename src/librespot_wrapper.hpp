#ifndef LIBRESPOT_WRAPPER_HPP
#define LIBRESPOT_WRAPPER_HPP

#include <string>

class LibrespotWrapper {
public:
    LibrespotWrapper(const std::string &lib_path);
    ~LibrespotWrapper();

    void play_track(const std::string &track_id);

private:
    void* handle;
    typedef void (*PlayTrackFunc)(const char*);
    PlayTrackFunc play_track_func;
};

#endif // LIBRESPOT_WRAPPER_HPP
