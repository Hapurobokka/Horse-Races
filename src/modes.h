#pragma once

#include "horse.h"

#include <vector>

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
	std::vector<Rectangle> map;
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

	virtual GameMode *update(GameContext &gc) = 0;
	virtual void render(GameContext &gc) = 0;
};

class RaceMode : public GameMode {
private:
	Timer go_label{};
	bool paused = false;
	bool race_started = false;
	bool victory = false;
	std::string winner{};

public:
	RaceMode(GameContext &gc);
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
    Horse* selected_uma = nullptr;

public:
	EditMode() {};
	GameMode* update(GameContext &gc) override;
	void render(GameContext &gc) override;
};
