#include "horse.h"

#include <iostream>
#include <utility>
#include "raylib.h"
#include "raymath.h"

using std::string;
using std::cout;

Horse::Horse(string n, string t) : name {std::move( n )}, radius(20) {
    cout << name << " creado.\n";
    t = "assets/images/" + t;
    texture = LoadTexture(t.c_str());
}


void Horse::render() {
    DrawTextureEx(
        texture,
        Vector2{position.x - radius, position.y - radius},
        0,
        texture.width / 6000.0,
        WHITE
    );
    int text_width = MeasureText(name.c_str(), 8);
    DrawText(
        name.c_str(),
        position.x - text_width / 2.0f,
        position.y + radius / 2.0f + 10,
        8,
        BLACK
    );
}

void Horse::accelerate() {
    position.x += speed.x;
    position.y += speed.y;
}

bool Horse::collide_with_border(Rectangle &b) {
    if (!CheckCollisionCircleRec(position, radius, b)) {
        return false;
    }

    int center_x = b.x + b.width / 2;
    int center_y = b.y + b.height / 2;

    int dx = position.x - center_x;
    int dy = position.y - center_y;

    if (abs(dx / b.width) > abs(dy / b.height)) {
        speed.x *= -1.0;
        if (dx > 0)
            position.x = b.x + b.width + radius;
        else
            position.x = b.x - radius;
    } else {
        speed.y *= -1.0;
        if (dy > 0)
            position.y = b.y + b.height + radius;
        else
            position.y = b.y - radius;
    }
    return true;
}

bool Horse::collide_with_horse(Horse *h) {
    Vector2 diff = Vector2Subtract(h->position, position);
    float dist = Vector2Length(diff);
    float min_dist = radius + h->radius;

    if (dist < min_dist && dist > 0.0f) {
        Vector2 normal = Vector2Scale(diff, 1.0f / dist);

        float overlap = (min_dist - dist) / 2.0f;
        position = Vector2Subtract(position, Vector2Scale(normal, overlap));
        h->position = Vector2Add(h->position, Vector2Scale(normal, overlap));

        Vector2 relative_velocity = Vector2Subtract(h->speed, speed);
        float v_along_normal = Vector2DotProduct(relative_velocity, normal);

        if (v_along_normal > 0) { return false; }

        Vector2 impulse = Vector2Scale(normal, v_along_normal);

        speed = Vector2Add(speed, impulse);
        h->speed = Vector2Subtract(h->speed, impulse);

        return true;
    }

    return false;
}

Horse::~Horse() {
    UnloadTexture(texture);
}
