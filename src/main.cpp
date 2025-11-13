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

enum class GameState {
    Menu,
    Race,
    Edit,
};

struct Goal {
    Vector2 position;
    Texture texture;
};


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

struct GameContext {
    GameState current_mode;
    vector<Horse*> horses;
    vector<Rectangle> map;
    Goal goal;
    Music ost;
    Sound boop;

    bool paused = false;
    bool race_started = false;
    bool victory = false;
    string winner {};

    Timer music_t = Timer{};
    Timer go_label = Timer{};

    void clean() {
        UnloadSound(boop);
        UnloadMusicStream(ost);
        for (auto h : horses) {
            h->clean();
            delete h;
            h = nullptr;
        }
        UnloadTexture(goal.texture);
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

void race_mode_logic(GameContext &gc) {
    UpdateMusicStream(gc.ost);
    if (gc.music_t.is_done() && !gc.race_started) {
        gc.race_started = true;
        gc.go_label.start(3);
        if (!IsMusicStreamPlaying(gc.ost))
            PlayMusicStream(gc.ost);
    }
    if (IsKeyPressed(KEY_SPACE) && !gc.victory && gc.race_started) gc.paused = !gc.paused; 

    if (!gc.paused && !gc.victory && gc.race_started) {
        for (auto h : gc.horses) {
            h->accelerate();
            for (auto b : gc.map)
                if (h->collide_with_border(b)) PlaySound(gc.boop);
            for (auto h2 : gc.horses)
                if (h->collide_with_horse(h2)) PlaySound(gc.boop);

            if (CheckCollisionCircles(h->get_position(), h->get_radius(), gc.goal.position, 10)) {
                gc.victory = true;
                gc.winner = h->get_name();
            }
        }
    }
}

void race_mode_render(GameContext &gc) {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        for (auto b : gc.map) { DrawRectangleRec(b, PURPLE); };
        DrawTextureEx(
            gc.goal.texture,
            Vector2{ gc.goal.position.x - 10, gc.goal.position.y - 10 },
            0.0f,
            gc.goal.texture.width / 25000.0f,
            WHITE
        );
        for (auto h : gc.horses){  h->render(); };
        if (gc.current_mode == GameState::Menu)
            DrawText("Press space to start", 350, 200, 30, GRAY);
        if (!gc.race_started && gc.current_mode == GameState::Race)
            DrawText("Ready?", 350, 200, 30, GRAY);
        if (!gc.go_label.is_done() && !gc.paused)
            DrawText("GO!", 350, 200, 30, GRAY);
        if (gc.paused)
            DrawText("Paused", 350, 200, 30, GRAY);
        if (gc.victory)
            DrawText(TextFormat("WINNER: %s", gc.winner.c_str()), 350, 200, 30, YELLOW);
        DrawFPS(10, 10);
    EndDrawing();
}

void welcome_mode_logic(GameContext &gc) {
    if (IsKeyPressed(KEY_SPACE)) {
        gc.current_mode = GameState::Race;
        gc.music_t.start(3);
    }
}

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WIDTH, HEIGHT, "Umamusume");
    SetTargetFPS(60);

    GameContext gc {};

    gc.boop = LoadSound("assets/music/collide.wav");
    gc.ost = LoadMusicStream("assets/music/versus.mp3");

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

    gc.horses = p_horses 
        | views::transform([](const tuple<string, string> t){
            return new Horse(get<0>(t), get<1>(t)); 
        }) | ranges::to<std::vector>();

    gc.map = {
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

    gc.goal = {Vector2{GetScreenWidth() - 40.0f, 40}, LoadTexture("assets/images/carrot.png")};
    gc.current_mode = GameState::Menu;

    InitAudioDevice();
    randomize_race(gc.horses);

    while (!WindowShouldClose()) {
    switch (gc.current_mode) {
        case GameState::Race:
            race_mode_logic(gc);
            race_mode_render(gc);
            break;
        case GameState::Menu:
            welcome_mode_logic(gc);
            race_mode_render(gc);
            break;
        default:
            cout << "No";
        }
    }

    gc.clean();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
