#include "modes.h"

#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

void Timer::start(double lf) {
    start_time = GetTime();
    lifetime = lf;
}

bool Timer::is_done() {
    return GetTime() - start_time >= lifetime;
}

double Timer::get_elapsed() { return GetTime() - start_time; }

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
				if (h->collide_with_border(*b)) PlaySound(gc.boop);
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
	for (auto b : gc.map) DrawRectangleRec(*b, PURPLE);

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

RaceMode::RaceMode() {};

GameMode *MenuMode::update(GameContext &gc) {
	if (button_race_pressed) {
		gc.music_t.start(3);
        std::cout << "INFO: Entering Race Mode\n";
		return new RaceMode();
    }

    if (button_edit_pressed) {
        std::cout << "INFO: Entering Edit Mode\n";
        return new EditMode();
    }

    return nullptr;
}

void MenuMode::render(GameContext &gc) {
    ClearBackground(RAYWHITE);
    for (auto b : gc.map) DrawRectangleRec(*b, PURPLE);

    DrawTextureEx(gc.goal.texture,
                  Vector2{gc.goal.position.x - 10, gc.goal.position.y - 10},
                  0.0f,
                  gc.goal.texture.width / 25000.0f, WHITE);

    for (auto h : gc.horses) h->render();

    if (GuiButton(Rectangle{275, 250, 200, 30}, "Start"))
        button_race_pressed = true;

    if (GuiButton(Rectangle{275, 280, 200, 30}, "Edit"))
        button_edit_pressed = true;


    DrawText("Press start to start", 250, 200, 30, GRAY);
    DrawFPS(10, 10);
}

bool EditMode::is_mouse_in_border() {
    return mouse_in_left_down ||
           mouse_in_right_down ||
           mouse_in_left_upper ||
           mouse_in_right_upper;
}

GameMode* EditMode::update(GameContext &gc) {
    Vector2 mouse = GetMousePosition();

    if (back_button_pressed) {
        std::cout << "INFO: Entering Menu Mode\n";
        return new MenuMode();
    }

    for (auto h : gc.horses) {
        if (CheckCollisionPointCircle(mouse, h->get_position(), h->get_radius())
            && IsMouseButtonDown(MOUSE_LEFT_BUTTON)
            && !mouse_in_uma
            && !is_mouse_in_border()) {
            std::cout << "Haz hecho click en " << h->get_name() << "\n";
            mouse_in_uma = true;
            selected_uma = h;
        }
    }

    for (auto b : gc.map) {
        if (CheckCollisionPointCircle(mouse, Vector2{ b->x, b->y }, 5)
            && IsMouseButtonDown(MOUSE_LEFT_BUTTON)
            && !mouse_in_left_upper
            && !mouse_in_uma
            && !is_mouse_in_border()) {
            std::cout << "Haz hecho clic en el borde superior de un rectangulo\n";
            mouse_in_left_upper = true;
            selected_rectangle = b;
        }

        if (CheckCollisionPointCircle(mouse, Vector2{ b->x, b->y + b->height }, 5)
            && IsMouseButtonDown(MOUSE_LEFT_BUTTON)
            && !mouse_in_left_down
            && !mouse_in_uma
            && !is_mouse_in_border()) {
            mouse_in_left_down = true;
            selected_rectangle = b;
        }

        if (CheckCollisionPointCircle(mouse, Vector2{ b->x + b->width, b->y }, 5)
            && IsMouseButtonDown(MOUSE_LEFT_BUTTON)
            && !mouse_in_uma
            && !is_mouse_in_border()) {
            mouse_in_right_upper = true;
            selected_rectangle = b;
        }

        if (CheckCollisionPointCircle(mouse, Vector2{ b->x + b->width, b->y + b->height }, 5)
            && IsMouseButtonDown(MOUSE_LEFT_BUTTON)
            && !mouse_in_uma
            && !is_mouse_in_border()) {
            mouse_in_right_down = true;
            selected_rectangle = b;
        }
    }

    if (mouse_in_uma && selected_uma != nullptr) {
        selected_uma->set_position(mouse);
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            mouse_in_uma = false;
            selected_uma = nullptr;
        }
    }

    if (mouse_in_left_upper && selected_rectangle != nullptr) {
        Vector2 pos_offset = Vector2Subtract(Vector2{ selected_rectangle->x, selected_rectangle->y }, mouse);
        selected_rectangle->x = mouse.x;
        selected_rectangle->y = mouse.y;
        selected_rectangle->width = selected_rectangle->width + pos_offset.x;
        selected_rectangle->height = selected_rectangle->height + pos_offset.y;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            mouse_in_left_upper = false;
            selected_rectangle = nullptr;
        }
    }

    if (mouse_in_left_down && selected_rectangle != nullptr) {
        Vector2 pos_offset = Vector2Subtract(Vector2{ selected_rectangle->x, selected_rectangle->y + selected_rectangle->height }, mouse);
        selected_rectangle->x = mouse.x;
        selected_rectangle->width = selected_rectangle->width + pos_offset.x;
        selected_rectangle->height = selected_rectangle->height - pos_offset.y;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            mouse_in_left_down = false;
            selected_rectangle = nullptr;
        }
    }

    if (mouse_in_right_upper && selected_rectangle != nullptr) {
        Vector2 pos_offset = Vector2Subtract(Vector2{ selected_rectangle->x + selected_rectangle->width, selected_rectangle->y }, mouse);
        selected_rectangle->width = selected_rectangle->width - pos_offset.x;
        selected_rectangle->height = selected_rectangle->height + pos_offset.y;
        selected_rectangle->y = mouse.y;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            mouse_in_right_upper = false;
            selected_rectangle = nullptr;
        }
    }

    if (mouse_in_right_down && selected_rectangle != nullptr) {
        Vector2 pos_offset = Vector2Subtract(Vector2{ selected_rectangle->x + selected_rectangle->width, selected_rectangle->y + selected_rectangle->height }, mouse);
        selected_rectangle->width = selected_rectangle->width - pos_offset.x;
        selected_rectangle->height = selected_rectangle->height - pos_offset.y;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            mouse_in_right_down = false;
            selected_rectangle = nullptr;
        }
    }

    return nullptr;
}

void EditMode::render(GameContext &gc) {
    ClearBackground(RAYWHITE);
    for (auto b : gc.map) {
        DrawRectangleRec(*b, PURPLE);
        DrawCircle(b->x, b->y, 5, BLUE);
        DrawCircle(b->x + b->width, b->y, 5, BLUE);
        DrawCircle(b->x, b->y + b->height, 5, BLUE);
        DrawCircle(b->x + b->width, b->y + b->height, 5, BLUE);
    };

    for (auto h : gc.horses) h->render();

    if (GuiButton(Rectangle{ 20, 20, 20, 20 }, "<"))
        back_button_pressed = true;

    DrawTextureEx(gc.goal.texture,
                  Vector2{gc.goal.position.x - 10, gc.goal.position.y - 10},
                  0.0f,
                  gc.goal.texture.width / 25000.0f,
                  WHITE);
}
