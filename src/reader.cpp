#include "reader.h"

#include "json.hpp"
#include "modes.h"
#include "raylib.h"

#include <algorithm>
#include <fstream>
#include <ranges>
#include <string>
#include <vector>

using json = nlohmann::json;
using std::string;
using std::vector;
using std::views::transform;
namespace fs = std::filesystem;

void reader::dump_map(GameContext& gc, const string& path) {
    std::ofstream f(path, std::ios::out | std::ios::trunc);
    json j;
    for (int i = 0; i < (int)gc.map.size(); i++) {
        j["map"][i]["x"] = gc.map[i].x;
        j["map"][i]["y"] = gc.map[i].y;
        j["map"][i]["width"] = gc.map[i].width;
        j["map"][i]["height"] = gc.map[i].height;
    }

    int hi = 0;
    for (const auto& h : gc.horses) {
        j["horse"][hi]["x"] = h->get_position().x;
        j["horse"][hi]["y"] = h->get_position().y;
        hi++;
    }

    j["goal"]["x"] = gc.goal.position.x;
    j["goal"]["y"] = gc.goal.position.y;

    f << j.dump();
    f.close();
}

void reader::read_map(GameContext& gc, const string& path) {
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

    int hi = 0;
    for (const auto& h : gc.horses) {
        float x = j["horse"][hi]["x"].get<float>();
        float y = j["horse"][hi]["y"].get<float>();
        h->set_position(Vector2{ x, y });
        hi++;
    }

    float x = j["goal"]["x"].get<float>();
    float y = j["goal"]["y"].get<float>();
    gc.goal.position = Vector2{ x, y };
}

string reader::get_paths_string(const vector<string>& map_route) {
    return std::ranges::fold_left(
        map_route, string{}, [](const string& acc, const string& path) {
            return acc.empty() ? path : acc + ";" + path;
        });
}

string reader::extract_name(string path) {
    string del = "assets/images/";
    path.erase(0, del.size());
    return path.substr(0, path.find('.')) |
           std::views::transform([](const char& c) { return toupper(c); }) |
           std::ranges::to<string>();
}
