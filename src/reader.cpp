#include "reader.h"

#include "json.hpp"
#include "modes.h"
#include "raylib.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <ranges>
#include <vector>

using json = nlohmann::json;
using std::string;
using std::views::transform;
using std::vector;
namespace fs = std::filesystem;

void reader::dump_map(GameContext& gc, const string &path) {
    std::ofstream f(path, std::ios::out | std::ios::trunc);
    json j;
    for (int i = 0; i < (int)gc.map.size(); i++) {
        j["map"][i]["x"] = gc.map[i].x;
        j["map"][i]["y"] = gc.map[i].y;
        j["map"][i]["width"] = gc.map[i].width;
        j["map"][i]["height"] = gc.map[i].height;
    }
    f << j.dump();
    f.close();
}

void reader::read_map(GameContext& gc, const string &path) {
    json j;
    std::ifstream f(path);
    f >> j;
    f.close();

    for (int i = 0; i < (int)j["map"].size(); i++) {
        float x = j["map"][i]["x"].get<float>();
        float y = j["map"][i]["y"].get<float>();
        float width = j["map"][i]["width"].get<float>();
        float height = j["map"][i]["height"].get<float>();

        gc.map.emplace_back(Rectangle{ x, y, width, height });
    }
}

vector<string> reader::get_map_list(const std::string &map_route) {
    return fs::directory_iterator(map_route) 
    | transform([](const fs::directory_entry &entry) { return entry.path().string(); }) 
    | std::views::filter([](const string &path) { return path.ends_with(".json"); })
    | std::ranges::to<vector>();
}

string reader::get_maps_string(const vector<string>& map_route) {
    return std::ranges::fold_left(
        map_route,
        string{},
        [](const string& acc, const string& path) {
            return acc.empty() ? path : acc + ";" + path;
        }
    );
}
