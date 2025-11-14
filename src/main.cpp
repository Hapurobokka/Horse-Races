#include "horse.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <random>
#include <ranges>
#include <raylib.h>
#include <string>
#include <tuple>
#include <vector>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#define WIDTH 800
#define HEIGHT 450

using namespace std;

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
    vector<Horse*> horses;
    vector<Rectangle> map;
    Goal goal;
    Music ost;
    Sound boop;

    Timer music_t = Timer{};

    ~GameContext() {
        UnloadSound(boop);
        UnloadMusicStream(ost);
        for (auto h : horses) {
            delete h;
            h = nullptr;
        }
        UnloadTexture(goal.texture);
    }
};

class GameMode {
public:
    virtual ~GameMode() = default;

    virtual GameMode* update(GameContext& gc) = 0;
    virtual void render(GameContext& gc) = 0;
};

class RaceMode : public GameMode {
private:
    Timer go_label {};
    bool paused = false;
    bool race_started = false;
    bool victory = false;
    string winner {};

public:
    void randomize_race(GameContext &gc) {
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

        for (auto h : gc.horses) {
            Vector2 new_pos = starting_positions.back();
            starting_positions.pop_back();
            int random_pos = rand() % possible_speeds.size();

            h->set_position(new_pos);
            h->set_speed(possible_speeds[random_pos]);
        }
    }

    RaceMode(GameContext &gc) {
        randomize_race(gc);
    }

    GameMode* update(GameContext& gc) override {
        UpdateMusicStream(gc.ost);
        if (gc.music_t.is_done() && !race_started) {
            race_started = true;
            go_label.start(3);
            if (!IsMusicStreamPlaying(gc.ost))
                PlayMusicStream(gc.ost);
        }
        if (IsKeyPressed(KEY_SPACE) && !victory && race_started)
            paused = !paused;

        if (!paused && !victory && race_started) {
            for (auto h : gc.horses) {
                h->accelerate();
                for (auto b : gc.map)
                    if (h->collide_with_border(b)) PlaySound(gc.boop);
                for (auto h2 : gc.horses)
                    if (h->collide_with_horse(h2)) PlaySound(gc.boop);

                if (CheckCollisionCircles (
                    h->get_position(), h->get_radius(), gc.goal.position, 10)
                ) {
                    victory = true;
                    winner = h->get_name();
                }
            }
        }

        return nullptr;
    }

    void render(GameContext &gc) override {
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
        if (!race_started)
            DrawText("Ready?", 350, 200, 30, GRAY);
        if (!go_label.is_done() && !paused)
            DrawText("GO!", 350, 200, 30, GRAY);
        if (paused)
            DrawText("Paused", 350, 200, 30, GRAY);
        if (victory)
            DrawText(TextFormat("WINNER: %s", winner.c_str()), 350, 200, 30, YELLOW);
        DrawFPS(10, 10);
    }
};

class MenuMode : public GameMode {
private:
    bool button_pushed = false;

public:
    MenuMode() {};

    GameMode* update(GameContext &gc) override {
        if (button_pushed) {
            gc.music_t.start(3);
            return new RaceMode(gc);
        } else {
            return nullptr;
        }
    }

    void render(GameContext &gc) override {
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
        if (GuiButton(Rectangle { 350, 250, 200, 30}, "Start")) {
            button_pushed = true;
        }
        DrawText("Press start to start", 350, 200, 30, GRAY);
        DrawFPS(10, 10);
    }
};

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

    InitAudioDevice();

    GameMode* current_state = new MenuMode();

    while (!WindowShouldClose()) {
        GameMode* next_state = current_state->update(gc);

        BeginDrawing();
        current_state->render(gc);
        EndDrawing();

        if (next_state != nullptr) {
            delete current_state;
            current_state = next_state;
        }
    }

    CloseAudioDevice();
    CloseWindow();
    return 0;
}
