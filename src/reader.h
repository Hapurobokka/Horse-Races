#pragma once

#include "modes.h"
#include <vector>
#include <filesystem>
#include <ranges>

namespace reader {
    void dump_map(GameContext& gc, const std::string& path);
    void read_map(GameContext& gc, const std::string& path);
    template<typename FilterFunc>
    std::vector<std::string> get_path_list(const std::string& map_route, FilterFunc filter) {
        return std::filesystem::directory_iterator(map_route) |
               std::views::transform([](const std::filesystem::directory_entry& entry) {
                   return entry.path().string();
               }) |
               std::views::filter(filter) |
               std::ranges::to<std::vector>();
    }

    std::string get_paths_string(const std::vector<std::string>& map_route);
}
