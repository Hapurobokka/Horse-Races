#pragma once

#include <iostream>
#include <raylib.h>

class Horse {
private:
    std::string name;
    Vector2 position;
    Vector2 speed;
    int radius;
    Texture texture;
public:
    Horse(std::string n, Texture t);

    void render();
    void clean();
    void accelerate();
    bool collide_with_border(Rectangle &b);
    bool collide_with_horse(Horse *h);

    std::string get_name() { return name; }
    Vector2 get_position() { return position; }
    int get_radius() { return radius; };
    void set_position(Vector2 new_pos) { position = new_pos; }
    void set_speed(Vector2 new_speed) { speed = new_speed; }
};
