#include "reader.h"

#include "json.hpp"
#include "modes.h"
#include "raylib.h"

#include <fstream>

using namespace std;
using json = nlohmann::json;

void reader::dump_map(GameContext &gc) {
    std::ofstream f("assets/tables/table.json");
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

void reader::read_map(GameContext &gc) {
    json j;
    std::ifstream f("assets/tables/table.json");
    f >> j;
    f.close();

    for (int i = 0; i < j["map"].size(); i++) {
        float x = j["map"][i]["x"].get<float>();
        float y = j["map"][i]["y"].get<float>();
        float width  = j["map"][i]["width"].get<float>();
        float height = j["map"][i]["height"].get<float>();

        gc.map.emplace_back(Rectangle{ x, y, width, height });
    }
}
