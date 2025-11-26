#pragma once

#include "horse.h"
#include "raylib.h"

#include <memory>
#include <vector>

// Timer para contar el tiempo.
class Timer {
  private:
    double start_time;
    double lifetime;

  public:
    void start(double lf);
    bool is_done() const;
    double get_elapsed() const;
};

struct Goal {
    Vector2 position;
    Texture texture;
};

class GameContext {
  public:
    std::vector<std::unique_ptr<Horse>> horses;
    std::vector<Rectangle> map;
    Goal goal;
    Music ost;
    Sound boop;

    Timer music_t = Timer{};

    int path_selected = 0;
    int prev_selected = 1;

    std::string paths_string;
    std::vector<std::string> file_paths;

    GameContext();

    ~GameContext() {
        UnloadSound(boop);
        UnloadMusicStream(ost);
        UnloadTexture(goal.texture);
    }
};

// Clase base para cada modo de juego.
class GameMode {
  public:
    virtual ~GameMode() = default;

    // Método responsable de manejar la lógica del juego.
    // Desde aquí viene cualquier cambio a otro modo.
    virtual std::unique_ptr<GameMode> update(GameContext& gc) = 0;
    // Método responsable de dibujar todo a la pantalla.
    virtual void render(GameContext& gc) = 0;
};

class RaceMode : public GameMode {
  private:
    // Idealmente cualquier estado que necesite tu modo entre bucles y no
    // necesita estar disponible para otros modos debería guardarse aquí.
    Timer go_label{};
    bool paused = false;
    bool race_started = false;
    bool victory = false;
    std::string winner;

  public:
    RaceMode() = default;
    std::unique_ptr<GameMode> update(GameContext& gc) override;
    void render(GameContext& gc) override;
};

class MenuMode : public GameMode {
  private:
    bool button_race_pressed = false;
    bool button_edit_pressed = false;
    bool button_saved_pressed = false;

  public:
    MenuMode();
    std::unique_ptr<GameMode> update(GameContext& gc) override;
    void render(GameContext& gc) override;
};

class EditMode : public GameMode {
  private:
    bool back_button_pressed = false;
    bool mouse_in_uma = false;
    bool mouse_in_goal = false;

    enum class GrabbedBorder {
        NONE,
        LEFT_UPPER,
        LEFT_DOWN,
        RIGHT_UPPER,
        RIGHT_DOWN,
        CENTER
    };

    GrabbedBorder mouse_in_border = EditMode::GrabbedBorder::NONE;
    Horse* selected_uma = nullptr;
    int i_rectangle{};

    bool check_if_mouse_in_point(Vector2& mouse, Vector2 point);
    void check_if_mouse_in_border(GameContext& gc, Vector2& mouse);
    void check_if_mouse_in_horse(GameContext& gc, Vector2& mouse);
    void move_border(GameContext& gc, Vector2& mouse);
    void move_horse(Vector2 mouse);
    void delete_border(GameContext& gc, Vector2 mouse);

  public:
    EditMode() = default;
    std::unique_ptr<GameMode> update(GameContext& gc) override;
    void render(GameContext& gc) override;
};
