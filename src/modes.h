#pragma once
#include <vector>
#include "horse.h"

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
	void randomize_race(GameContext &gc);
	GameMode *update(GameContext &gc) override;
	void render(GameContext &gc) override;
};

class MenuMode : public GameMode {
private:
	bool button_pushed = false;

public:
	MenuMode() {};
	GameMode* update(GameContext &gc) override;
	void render(GameContext &gc) override;
};
