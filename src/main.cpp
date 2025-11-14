#include "horse.h"

#include <cmath>
#include <cstdlib>
#include <ranges>
#include <raylib.h>
#include <string>
#include <tuple>
#include <vector>

#include "modes.h"

#define WIDTH 800
#define HEIGHT 450

using namespace std;

int main() {
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(WIDTH, HEIGHT, "Umamusume");
	SetTargetFPS(60);

	GameContext gc{};

	gc.boop = LoadSound("assets/music/collide.wav");
	gc.ost = LoadMusicStream("assets/music/versus.mp3");

	vector<tuple<string, string>> p_horses = {
		{"SPCWK", "spcwk.png"},       {"MAMBO", "mambo.png"},
		{"MJMQ", "mjmq.png"},         {"TOTE", "teto.png"},
		{"BAKUSHIN", "bakushin.png"}, {"CHIYO", "chiyono.png"},
		{"GLSP", "gold.png"},         {"SILSUZ", "silsuz.png"}};

	gc.horses = p_horses | views::transform([](const tuple<string, string> t) {
				return new Horse(get<0>(t), get<1>(t));
				}) |
				ranges::to<std::vector>();

	gc.map = {
		Rectangle{0, 0, (GetScreenWidth() - 20.0f), 20},
		Rectangle{0, 20, 20, GetScreenHeight() - 20.0f},
		Rectangle{0, (GetScreenHeight() - 20.0f), (GetScreenWidth() - 20.0f), 20},
		Rectangle{(GetScreenWidth() - 20.0f), 0, 20,
				static_cast<float>(GetScreenHeight())},
		Rectangle{200, 20, 60, 200},
		Rectangle{340, 20, 60, 240},
		Rectangle{440, 20, 60, 40},
		Rectangle{540, 20, 140, 60},
		Rectangle{480, 140, 300, 40},
		Rectangle{20, (GetScreenHeight() - 180.0f), 100, 40},
		Rectangle{460, (GetScreenHeight() - 180.0f), 240, 40},
		Rectangle{200, (GetScreenHeight() - 100.0f), 60, 80},
		Rectangle{340, (GetScreenHeight() - 60.0f), 60, 40},
		Rectangle{460, (GetScreenHeight() - 60.0f), 240, 40},
	};


	InitAudioDevice();

	GameMode *current_state = new MenuMode();

	while (!WindowShouldClose()) {
		GameMode *next_state = current_state->update(gc);

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
