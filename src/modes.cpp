#include "modes.h"

#include <raylib.h>
#include <vector>
#include <algorithm>
#include <random>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

using std::vector;

void Timer::start(double lf) {
    start_time = GetTime();
    lifetime = lf;
}

bool Timer::is_done() {
    return GetTime() - start_time >= lifetime;
}

double Timer::get_elapsed() { return GetTime() - start_time; }

RaceMode::RaceMode(GameContext &gc) { randomize_race(gc); }

void RaceMode::randomize_race(GameContext &gc) {
    vector<Vector2> starting_positions = {
        Vector2{80, 50},  Vector2{140, 50},  Vector2{80, 100}, Vector2{140, 100},
        Vector2{80, 150}, Vector2{140, 150}, Vector2{80, 200}, Vector2{140, 200},
    };

    srand(time(0));
    std::random_device rd;
    std::default_random_engine rng(rd());
    shuffle(starting_positions.begin(), starting_positions.end(), rng);

    vector<Vector2> possible_speeds = {
        Vector2{2.0, 1.0},   Vector2{-2.0, 1.0},  Vector2{2.0, -1.0},
        Vector2{-2.0, -1.0}, Vector2{1.0, 2.0},   Vector2{-1.0, 2.0},
        Vector2{1.0, -2.0},  Vector2{-1.0, -2.0}, Vector2{1.5, 1.5},
        Vector2{-1.5, 1.5},  Vector2{1.5, -1.5},  Vector2{-1.5, -1.5},
    };

    for (auto h : gc.horses) {
        Vector2 new_pos = starting_positions.back();
        starting_positions.pop_back();
        int random_pos = rand() % possible_speeds.size();

        h->set_position(new_pos);
        h->set_speed(possible_speeds[random_pos]);
    }
}

GameMode* RaceMode::update(GameContext &gc) {
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

			if (CheckCollisionCircles(h->get_position(), h->get_radius(), gc.goal.position, 10)) {
				victory = true;
				winner = h->get_name();
			}
		}
	}

	return nullptr;
}

void RaceMode::render(GameContext &gc) {
	ClearBackground(RAYWHITE);
	for (auto b : gc.map) DrawRectangleRec(b, PURPLE);

	DrawTextureEx(gc.goal.texture,
                  Vector2{gc.goal.position.x - 10, gc.goal.position.y - 10},
                  0.0f,
                  gc.goal.texture.width / 25000.0f, WHITE);

    for (auto h : gc.horses) h->render();

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

GameMode *MenuMode::update(GameContext &gc) {
	if (button_pushed) {
		gc.music_t.start(3);
		return new RaceMode(gc);
	} else {
		return nullptr;
	}
}

void MenuMode::render(GameContext &gc) {
    ClearBackground(RAYWHITE);
    for (auto b : gc.map) {
        DrawRectangleRec(b, PURPLE);
    };

    DrawTextureEx(gc.goal.texture,
                  Vector2{gc.goal.position.x - 10, gc.goal.position.y - 10},
                  0.0f,
                  gc.goal.texture.width / 25000.0f, WHITE);

    if (GuiButton(Rectangle{275, 250, 200, 30}, "Start")) {
        button_pushed = true;
    }
    DrawText("Press start to start", 250, 200, 30, GRAY);
    DrawFPS(10, 10);
}
