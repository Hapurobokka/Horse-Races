#pragma once

#include "modes.h"
#include <vector>

namespace reader {
    void dump_map(GameContext &gc, const std::string &path);
    void read_map(GameContext &gc, const std::string &path);
    std::vector<std::string> get_map_list(const std::string &map_route);
    std::string get_maps_string(const std::vector<std::string>& map_route);
}
