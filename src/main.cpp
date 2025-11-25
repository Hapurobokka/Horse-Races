#include "raylib.h"
#include <algorithm>
#include <cstdlib>
#include <memory>
#include <random>
#include <ranges>
#include <string>
#include <tuple>
#include <vector>

#include "horse.h"
#include "modes.h"
#include "reader.h"

#define WIDTH 800
#define HEIGHT 450

using std::string;
using std::tuple;
using std::unique_ptr;
using std::vector;

void randomize_race(GameContext& gc) {
    vector<Vector2> starting_positions = {
        Vector2{ 80, 50 },   Vector2{ 140, 50 },  Vector2{ 80, 100 },
        Vector2{ 140, 100 }, Vector2{ 80, 150 },  Vector2{ 140, 150 },
        Vector2{ 80, 200 },  Vector2{ 140, 200 },
    };

    srand(time(nullptr));
    std::random_device rd;
    std::default_random_engine rng(rd());
    shuffle(starting_positions.begin(), starting_positions.end(), rng);

    vector<Vector2> possible_speeds = {
        Vector2{ 2.0, 1.0 },   Vector2{ -2.0, 1.0 },  Vector2{ 2.0, -1.0 },
        Vector2{ -2.0, -1.0 }, Vector2{ 1.0, 2.0 },   Vector2{ -1.0, 2.0 },
        Vector2{ 1.0, -2.0 },  Vector2{ -1.0, -2.0 }, Vector2{ 1.5, 1.5 },
        Vector2{ -1.5, 1.5 },  Vector2{ 1.5, -1.5 },  Vector2{ -1.5, -1.5 },
    };

    for (const auto& h : gc.horses) {
        Vector2 new_pos = starting_positions.back();
        starting_positions.pop_back();
        int random_pos = rand() % possible_speeds.size();

        h->set_position(new_pos);
        h->set_speed(possible_speeds[random_pos]);
    }
}

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WIDTH, HEIGHT, "Horse Racing");
    SetTargetFPS(60);

    // Creamos un "contexto global".
    GameContext gc{};

    gc.boop = LoadSound("assets/music/collide.wav");
    gc.ost = LoadMusicStream("assets/music/versus.mp3");

    vector<tuple<string, string>> p_horses = {
        { "SPCWK", "spcwk.png" },       { "MAMBO", "mambo.png" },
        { "MJMQ", "mjmq.png" },         { "TOTE", "teto.png" },
        { "BAKUSHIN", "bakushin.png" }, { "CHIYO", "chiyono.png" },
        { "GLSP", "gold.png" },         { "SILSUZ", "silsuz.png" }
    };

    gc.horses = p_horses | std::views::transform([](const auto& t) {
                    return make_unique<Horse>(get<0>(t), get<1>(t));
                }) |
                std::ranges::to<std::vector>();

    InitAudioDevice();

    gc.goal = Goal{ .position = Vector2{ GetScreenWidth() - 60.0F, 60 },
                    .texture = LoadTexture("assets/images/carrot.png") };

    randomize_race(gc);

    // Creamos el primer modo de todos
    unique_ptr<GameMode> current_state(new MenuMode());

    while (!WindowShouldClose()) {
        // El método update puede devolver un nuevo modo.
        unique_ptr<GameMode> next_state(current_state->update(gc));

        // En este paso se dibuja toda la pantalla.
        BeginDrawing();
        current_state->render(gc);
        EndDrawing();

        if (next_state != nullptr) {
            // Si recibimos un nuevo estado, el actual es reemplazado.
            current_state.swap(next_state);
        }
    }

    CloseAudioDevice();
    CloseWindow();
    return 0;
}
