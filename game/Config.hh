#ifndef STRAT_GAME_CONFIG_HH
#define STRAT_GAME_CONFIG_HH

#include <cstring>

struct Config {
    Config()
        : screenWidth(640)
        , screenHeight(480)
        , fullscreen(false) {
    }
        
    size_t screenWidth;
    size_t screenHeight;
    bool fullscreen;
};

#endif
