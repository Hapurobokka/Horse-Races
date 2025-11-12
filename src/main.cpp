#include "horse.h"

#include <algorithm>
#include <bits/stdc++.h>
#include <cmath>
#include <cstdlib>
#include <random>
#include <ranges>
#include <raylib.h>
#include <raygui.h>
#include <string>
#include <tuple>
#include <vector>

#define RAYGUI_IMPLEMENTATION
#define WIDTH 800
#define HEIGHT 450

using namespace std;

class Timer {
    double start_time;
    double lifetime;

public:
    void start(double lf) {
        start_time = GetTime();
        lifetime = lf;
    }

    bool is_done() {
        return GetTime() - start_time >= lifetime;
    }

    double get_elapsed() {
        return GetTime() - start_time;
    }
};

void randomize_race(vector<Horse*> &horses) {
    vector<Vector2> starting_positions = {
        Vector2{ 80, 50 },
        Vector2{ 140, 50 },
        Vector2{ 80, 100 },
        Vector2{ 140, 100 },
        Vector2{ 80, 150 },
        Vector2{ 140, 150 },
        Vector2{ 80, 200 },
        Vector2{ 140, 200 },
    };

    srand(time(0));
    random_device rd;
    default_random_engine rng(rd());
    shuffle(starting_positions.begin(), starting_positions.end(), rng);

    vector<Vector2> possible_speeds = {
        Vector2{ 2.0, 1.0 },
        Vector2{ -2.0, 1.0 },
        Vector2{ 2.0, -1.0 },
        Vector2{ -2.0, -1.0 },
        Vector2{ 1.0, 2.0 },
        Vector2{ -1.0, 2.0 },
        Vector2{ 1.0, -2.0 },
        Vector2{ -1.0, -2.0 },
        Vector2{ 1.5, 1.5 },
        Vector2{ -1.5, 1.5 },
        Vector2{ 1.5, -1.5 },
        Vector2{ -1.5, -1.5 },
    };

    for (auto h : horses) {
        Vector2 new_pos = starting_positions.back();
        starting_positions.pop_back();
        int random_pos = rand() % possible_speeds.size();

        h->set_position(new_pos);
        h->set_speed(possible_speeds[random_pos]);
    }
}

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WIDTH, HEIGHT, "Umamusume");
    SetTargetFPS(60);
    InitAudioDevice();

    vector<tuple<string, string>> p_horses = {
        { "SPCWK", "spcwk.png" },
        { "MAMBO", "mambo.png" },
        { "MJMQ",  "mjmq.png" },
        { "TOTE", "teto.png" },
        { "BAKUSHIN", "bakushin.png" },
        { "CHIYO", "chiyono.png"},
        { "GLSP", "gold.png" },
        { "SILSUZ", "silsuz.png" }
    };

    vector<Horse*> horses = p_horses 
        | views::transform([](const tuple<string, string> t){
            string name, text;
            tie(name, text) = t;
            text = "assets/images/" + text;
            return new Horse(name, LoadTexture(text.c_str())); 
        }) 
        | ranges::to<std::vector>();

    vector<Rectangle> all_bounds {
        Rectangle{ 0, 0, (GetScreenWidth() - 20.0f), 20 },
        Rectangle{ 0, 20, 20, GetScreenHeight() - 20.0f },
        Rectangle{ 0, (GetScreenHeight() - 20.0f), (GetScreenWidth() - 20.0f), 20 },
        Rectangle{ (GetScreenWidth() - 20.0f), 0, 20, static_cast<float>(GetScreenHeight()) },
        Rectangle{ 200, 20, 60, 200 },
        Rectangle{ 340, 20, 60, 240 },
        Rectangle{ 440, 20, 60, 40 },
        Rectangle{ 540, 20, 140, 60 },
        Rectangle{ 480, 140, 300, 40 },
        Rectangle{ 20, (GetScreenHeight() - 180.0f), 100, 40 },
        Rectangle{ 460, (GetScreenHeight() - 180.0f), 240, 40 },
        Rectangle{ 20, (GetScreenHeight() - 100.0f), 100, 80 },
        Rectangle{ 200, (GetScreenHeight() - 100.0f), 60, 80 },
        Rectangle{ 340, (GetScreenHeight() - 60.0f), 60, 40 },
        Rectangle{ 460, (GetScreenHeight() - 60.0f), 240, 40 },
    };

    Vector2 goal_position = Vector2{GetScreenWidth() - 40.0f, 40};
    Texture carrot = LoadTexture("assets/images/carrot.png");

    randomize_race(horses);

    Sound boop = LoadSound("assets/music/collide.wav");
    Music ost = LoadMusicStream("assets/music/versus.mp3");

    bool paused = false;
    bool race_started = false;
    bool victory = false;
    string winner {};

    Timer music_t = Timer{};
    Timer go_label = Timer{};

    music_t.start(3);

    while (!WindowShouldClose()) {
        UpdateMusicStream(ost);
        if (music_t.is_done() && !race_started) {
            race_started = true;
            go_label.start(3);
            if (!IsMusicStreamPlaying(ost))
                PlayMusicStream(ost);
        }
        if (IsKeyPressed(KEY_SPACE) && !victory && race_started) paused = !paused; 

        if (!paused && !victory && race_started) {
            for (auto h : horses) {
                h->accelerate();
                for (auto b : all_bounds)
                    if (h->collide_with_border(b)) PlaySound(boop);
                for (auto h2 : horses)
                    if (h->collide_with_horse(h2)) PlaySound(boop);

                if (CheckCollisionCircles(h->get_position(), h->get_radius(), goal_position, 10)) {
                    victory = true;
                    winner = h->get_name();
                }
            }
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            for (auto b : all_bounds) { DrawRectangleRec(b, PURPLE); };
            DrawTextureEx(
                carrot,
                Vector2{ goal_position.x - 10, goal_position.y - 10 },
                0.0,
                carrot.width / 25000.0,
                WHITE
            );
            for (auto h : horses){  h->render(); };
            if (!race_started)
                DrawText("Ready?", 350, 200, 30, GRAY);
            if (!go_label.is_done())
                DrawText("GO!", 350, 200, 30, GRAY);
            if (paused)
                DrawText("Paused", 350, 200, 30, GRAY);
            if (victory)
                DrawText(TextFormat("WINNER: %s", winner.c_str()), 350, 200, 30, YELLOW);
            DrawFPS(10, 10);
        EndDrawing();
    }

    for (auto h : horses) {
        h->clean();
        delete h;
        h = nullptr;
    }
    UnloadSound(boop);
    UnloadMusicStream(ost);
    UnloadTexture(carrot);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
