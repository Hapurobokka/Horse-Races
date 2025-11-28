#pragma once

#include "raylib.h"
#include <string>
#include <utility>

class Horse {
  private:
    std::string name;
    Texture texture;
    Vector2 position;
    Vector2 speed;
    int radius;

  public:
    Horse(std::string n, const std::string &t);
    ~Horse();

    void render();
    void portrait_render(Vector2 pos);
    void accelerate();
    void swap_texture(const std::string &texture_path);
    bool collide_with_border(Rectangle& b);
    bool collide_with_horse(Horse* h);

    std::string get_name() { return name; }
    Vector2 get_position() { return position; }
    int get_radius() { return radius; };

    void set_position(Vector2 new_pos) { position = new_pos; }
    void set_speed(Vector2 new_speed) { speed = new_speed; }
    void set_name(std::string new_name) { name = std::move(new_name); }
};
