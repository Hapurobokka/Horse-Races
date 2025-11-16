#pragma once

#include "horse.h"
#include "raylib.h"

#include <vector>

// Timer para contar el tiempo.
class Timer {
private:
	double start_time;
	double lifetime;

public:
	void start(double lf);
	bool is_done();
	double get_elapsed();
};

struct Goal {
    Vector2 position;
    Texture texture;
};

struct GameContext {
	std::vector<Horse *> horses;
	std::vector<Rectangle *> map;
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

// Clase base para cada modo de juego.
class GameMode {
public:
	virtual ~GameMode() = default;

    // Método responsable de manejar la lógica del juego.
    // Desde aquí viene cualquier cambio a otro modo.
	virtual GameMode *update(GameContext &gc) = 0;
    // Método responsable de dibujar todo a la pantalla.
	virtual void render(GameContext &gc) = 0;
};

class RaceMode : public GameMode {
private:
    // Idealmente cualquier estado que necesite tu modo entre bucles y no
    // necesita estar disponible para otros modos debería guardarse aquí.
	Timer go_label{};
	bool paused = false;
	bool race_started = false;
	bool victory = false;
	std::string winner{};

public:
	RaceMode();
	GameMode *update(GameContext &gc) override;
	void render(GameContext &gc) override;
};

class MenuMode : public GameMode {
private:
	bool button_race_pressed = false;
    bool button_edit_pressed = false;

public:
	MenuMode() {};
	GameMode* update(GameContext &gc) override;
	void render(GameContext &gc) override;
};

class EditMode : public GameMode {
private:
    bool back_button_pressed = false;
    bool mouse_in_uma = false;

    enum class GrabbedBorder {
        None,
        LeftUpper,
        LeftDown,
        RightUpper,
        RightDown
    };

    GrabbedBorder mouse_in_border = EditMode::GrabbedBorder::None;
    Horse* selected_uma = nullptr;
    Rectangle* selected_rectangle = nullptr;
    void check_if_mouse_in_border(GameContext &gc, Vector2 mouse);
    void check_if_mouse_in_horse(GameContext &gc, Vector2 mouse);
    void move_border(Vector2 mouse);
    void move_horse(Vector2 mouse);

public:
	EditMode() {};
	GameMode* update(GameContext &gc) override;
	void render(GameContext &gc) override;
};
