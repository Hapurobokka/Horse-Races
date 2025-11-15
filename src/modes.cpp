#include "modes.h"

#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

void Timer::start(double lf) {
    start_time = GetTime();
    lifetime = lf;
}

bool Timer::is_done() {
    return GetTime() - start_time >= lifetime;
}

double Timer::get_elapsed() { return GetTime() - start_time; }

RaceMode::RaceMode(GameContext &gc) {}

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

			if (CheckCollisionCircles(h->get_position(),
                                      h->get_radius(),
                                      gc.goal.position, 10)) {
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
	if (button_race_pressed) {
		gc.music_t.start(3);
        std::cout << "INFO: Entering Race Mode\n";
		return new RaceMode(gc);
    }

    if (button_edit_pressed) {
        std::cout << "INFO: Entering Edit Mode\n";
        return new EditMode();
    }

    return nullptr;
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

    if (GuiButton(Rectangle{275, 250, 200, 30}, "Start"))
        button_race_pressed = true;

    if (GuiButton(Rectangle{275, 280, 200, 30}, "Edit"))
        button_edit_pressed = true;


    DrawText("Press start to start", 250, 200, 30, GRAY);
    DrawFPS(10, 10);
}

GameMode* EditMode::update(GameContext &gc) {
    if (back_button_pressed) {
        std::cout << "INFO: Entering Menu Mode\n";
        return new MenuMode();
    }

    return nullptr;
}

void EditMode::render(GameContext &gc) {
    ClearBackground(RAYWHITE);
    for (auto b : gc.map) {
        DrawRectangleRec(b, PURPLE);
    };

    for (auto h : gc.horses) h->render();

    if (GuiButton(Rectangle{ 20, 20, 20, 20 }, "<"))
        back_button_pressed = true;

    DrawTextureEx(gc.goal.texture,
                  Vector2{gc.goal.position.x - 10, gc.goal.position.y - 10},
                  0.0f,
                  gc.goal.texture.width / 25000.0f, WHITE);
}
