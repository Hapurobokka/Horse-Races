#include "raylib.h"

#include <cstdlib>
#include <memory>

#include "modes.h"

#define WIDTH 800
#define HEIGHT 450

using std::string;
using std::tuple;
using std::unique_ptr;
using std::vector;

void randomize_speed(GameContext& gc) {
    srand(time(nullptr));

    vector<Vector2> possible_speeds = {
        Vector2{ 2.0, 1.0 },   Vector2{ -2.0, 1.0 },  Vector2{ 2.0, -1.0 },
        Vector2{ -2.0, -1.0 }, Vector2{ 1.0, 2.0 },   Vector2{ -1.0, 2.0 },
        Vector2{ 1.0, -2.0 },  Vector2{ -1.0, -2.0 }, Vector2{ 1.5, 1.5 },
        Vector2{ -1.5, 1.5 },  Vector2{ 1.5, -1.5 },  Vector2{ -1.5, -1.5 },
    };

    for (const auto& h : gc.horses) {
        int random_pos = rand() % possible_speeds.size();
        h->set_speed(possible_speeds[random_pos]);
    }
}

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WIDTH, HEIGHT, "Horse Racing");
    SetTargetFPS(60);

    // Creamos un "contexto global".

    InitAudioDevice();
    GameContext gc{};
    randomize_speed(gc);

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
