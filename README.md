# spotless
Lightweight Spotify terminal client.
Control your Spotify account directly from the command line.
Play music, search for tracks, manage playlists, and more—all
without the need for a heavy app or GUI.

## Installation

### Linux / Mac
```bash
# clone repo
git clone https://github.com/gavrh/spotless.git && cd spotless

# build
mkdir build && cd build

cmake .. && make

# link to bin directory
sudo ln -sf "$(pwd)/spotless" /usr/local/bin/spotless
```

### Windows
coming soon...
