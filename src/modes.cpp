#include "modes.h"
#include "reader.h"

#include <memory>
#include <print>
#include <ranges>
#include <tuple>
#include <vector>

#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

using std::string;
using std::tuple;
using std::unique_ptr;
using std::vector;

GameContext::GameContext() {
    vector<tuple<string, string>> p_horses = {
        { "SPCWK", "spcwk.png" },       { "MAMBO", "mambo.png" },
        { "MJMQ", "mjmq.png" },         { "TOTE", "teto.png" },
        { "BAKUSHIN", "bakushin.png" }, { "CHIYO", "chiyono.png" },
        { "GLSP", "gold.png" },         { "SILSUZ", "silsuz.png" }
    };

    horses = p_horses | std::views::transform([](const auto& t) {
                 return make_unique<Horse>(get<0>(t), get<1>(t));
             }) | std::ranges::to<std::vector>();

    goal = Goal{ .position = Vector2{ GetScreenWidth() - 60.0F, 60 },
                 .texture = LoadTexture("assets/images/carrot.png") };

    file_paths = reader::get_map_list("assets/tables");
    paths_string = reader::get_maps_string(file_paths);

    std::println("INFO: Paths cargados: {:}", paths_string);

    menu_song = LoadMusicStream("assets/music/menu.mp3");
    PlayMusicStream(menu_song);
}

void Timer::start(double lf) {
    start_time = GetTime();
    lifetime = lf;
}

bool Timer::is_done() const {
    return GetTime() - start_time >= lifetime;
}

double Timer::get_elapsed() const {
    return GetTime() - start_time;
}

RaceMode::RaceMode() {
    boop = LoadSound("assets/music/collide.wav");
    SetSoundVolume(boop, 0.3);
    ost = LoadMusicStream("assets/music/versus.mp3");
    music_t.start(3);
}

unique_ptr<GameMode> RaceMode::update(GameContext& gc) {
    UpdateMusicStream(ost);
    if (music_t.is_done() && !race_started) {
        race_started = true;
        go_label.start(3);

        if (!IsMusicStreamPlaying(ost)) {
            PlayMusicStream(ost);
            race_music = true;
        }
    }

    if (IsKeyPressed(KEY_SPACE) && !victory && race_started) {
        paused = !paused;
    }

    if (!paused && !victory && race_started) {
        for (const auto& h : gc.horses) {
            h->accelerate();
            for (auto b : gc.map) {
                if (h->collide_with_border(b)) {
                    PlaySound(boop);
                }
            }
            for (const auto& h2 : gc.horses) {
                if (h->collide_with_horse(h2.get())) {
                    PlaySound(boop);
                }
            }

            if (CheckCollisionCircles(
                    h->get_position(), h->get_radius(), gc.goal.position, 10)) {
                victory = true;
                winner = h->get_name();
            }
        }
    }

    if (victory && race_music) {
        if (IsMusicStreamPlaying(ost)) {
            StopMusicStream(ost);
        }
        UnloadMusicStream(ost);
        race_music = false;
        ost = LoadMusicStream("assets/music/victory.mp3");
        if (!IsMusicStreamPlaying(ost)) {
            PlayMusicStream(ost);
        }
    }

    return nullptr;
}

void RaceMode::render(GameContext& gc) {
    ClearBackground(RAYWHITE);
    for (auto b : gc.map) {
        DrawRectangleRec(b, PURPLE);
    }

    DrawTextureEx(gc.goal.texture,
                  Vector2{ gc.goal.position.x - 10, gc.goal.position.y - 10 },
                  0.0F,
                  gc.goal.texture.width / 25000.0F,
                  WHITE);

    for (const auto& h : gc.horses) {
        h->render();
    }

    if (!race_started) {
        DrawText("Ready?", 350, 200, 30, GRAY);
    }
    if (!go_label.is_done() && !paused) {
        DrawText("GO!", 350, 200, 30, GRAY);
    }
    if (paused) {
        DrawText("Paused", 350, 200, 30, GRAY);
    }
    if (victory) {
        DrawText(
            TextFormat("WINNER: %s", winner.c_str()), 350, 200, 30, YELLOW);
    }
    DrawFPS(10, 10);
}

unique_ptr<GameMode> MenuMode::update(GameContext& gc) {
    UpdateMusicStream(gc.menu_song);
    if (button_race_pressed) {
        std::println("INFO: Entering Race Mode");
        StopMusicStream(gc.menu_song);
        return std::make_unique<RaceMode>();
    }

    if (button_edit_pressed) {
        std::println("INFO: Entering Edit Mode");
        return std::make_unique<EditMode>();
    }

    if (button_saved_pressed) {
        reader::dump_map(gc, gc.file_paths[gc.path_selected]);
        std::println("Mapa {:} guardado", gc.file_paths[gc.path_selected]);
        button_saved_pressed = false;
    }

    if (gc.path_selected != gc.prev_selected) {
        gc.map.clear();
        reader::read_map(gc, gc.file_paths[gc.path_selected]);
        std::println("Mapa {:} cargado", gc.file_paths[gc.path_selected]);

        gc.prev_selected = gc.path_selected;
    }

    if (button_picture_pressed) {
        return std::make_unique<PictureMode>();
    }

    return nullptr;
}

void MenuMode::render(GameContext& gc) {
    ClearBackground(RAYWHITE);
    for (auto b : gc.map) {
        DrawRectangleRec(b, PURPLE);
    }

    DrawTextureEx(gc.goal.texture,
                  Vector2{ gc.goal.position.x - 10, gc.goal.position.y - 10 },
                  0.0F,
                  gc.goal.texture.width / 25000.0F,
                  WHITE);

    for (const auto& h : gc.horses) {
        h->render();
    }

    if (GuiButton(Rectangle{ 275, 250, 200, 30 }, "Start")) {
        button_race_pressed = true;
    }

    if (GuiButton(Rectangle{ 275, 280, 200, 30 }, "Edit")) {
        button_edit_pressed = true;
    }

    GuiComboBox(Rectangle{ 275, 320, 200, 30 },
                gc.paths_string.c_str(),
                &gc.path_selected);

    if (GuiButton(Rectangle{ 275, 360, 200, 30 }, "Save")) {
        button_saved_pressed = true;
    }

    if (GuiButton(Rectangle{ 275, 395, 200, 20 }, "Picture")) {
        button_picture_pressed = true;
    }

    DrawText("Press start to start", 250, 200, 30, GRAY);
    DrawFPS(10, 10);
}

void EditMode::move_horse(Vector2 mouse) {
    if (mouse_in_uma && selected_uma != nullptr) {
        selected_uma->set_position(mouse);
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            mouse_in_uma = false;
            selected_uma = nullptr;
        }
    }
}

void EditMode::move_border(GameContext& gc, Vector2& mouse) {
    Vector2 pos_offset{};

    switch (mouse_in_border) {
        case GrabbedBorder::LEFT_UPPER:
            pos_offset = Vector2Subtract(
                Vector2{ gc.map[i_rectangle].x, gc.map[i_rectangle].y }, mouse);
            gc.map[i_rectangle].x = mouse.x;
            gc.map[i_rectangle].y = mouse.y;
            gc.map[i_rectangle].width += pos_offset.x;
            gc.map[i_rectangle].height += pos_offset.y;
            break;
        case GrabbedBorder::LEFT_DOWN:
            pos_offset = Vector2Subtract(
                Vector2{ gc.map[i_rectangle].x,
                         gc.map[i_rectangle].y + gc.map[i_rectangle].height },
                mouse);
            gc.map[i_rectangle].x = mouse.x;
            gc.map[i_rectangle].width += pos_offset.x;
            gc.map[i_rectangle].height -= pos_offset.y;
            break;
        case GrabbedBorder::RIGHT_UPPER:
            pos_offset = Vector2Subtract(
                Vector2{ gc.map[i_rectangle].x + gc.map[i_rectangle].width,
                         gc.map[i_rectangle].y },
                mouse);
            gc.map[i_rectangle].width -= pos_offset.x;
            gc.map[i_rectangle].height += pos_offset.y;
            gc.map[i_rectangle].y = mouse.y;
            break;
        case GrabbedBorder::RIGHT_DOWN:
            pos_offset = Vector2Subtract(
                Vector2{ gc.map[i_rectangle].x + gc.map[i_rectangle].width,
                         gc.map[i_rectangle].y + gc.map[i_rectangle].height },
                mouse);
            gc.map[i_rectangle].width -= pos_offset.x;
            gc.map[i_rectangle].height -= pos_offset.y;
            break;
        case GrabbedBorder::CENTER:
            pos_offset = Vector2Subtract(
                Vector2{ gc.map[i_rectangle].x +
                             (gc.map[i_rectangle].width / 2.0F),
                         gc.map[i_rectangle].y +
                             (gc.map[i_rectangle].height / 2.0F) },
                mouse);
            gc.map[i_rectangle].x -= pos_offset.x;
            gc.map[i_rectangle].y -= pos_offset.y;
            break;
        case GrabbedBorder::NONE:
            return;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        mouse_in_border = GrabbedBorder::NONE;
    }
}

bool EditMode::check_if_mouse_in_point(Vector2& mouse, Vector2 point) {
    return (CheckCollisionPointCircle(mouse, point, 5) &&
            mouse_in_border == GrabbedBorder::NONE && !mouse_in_uma);
}

void EditMode::check_if_mouse_in_border(GameContext& gc, Vector2& mouse) {
    for (int i = 0; i < (int)gc.map.size(); i++) {
        if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            return;
        }

        if (check_if_mouse_in_point(mouse,
                                    Vector2{ gc.map[i].x, gc.map[i].y })) {
            mouse_in_border = GrabbedBorder::LEFT_UPPER;
            i_rectangle = i;
        }

        if (check_if_mouse_in_point(
                mouse,
                Vector2{ gc.map[i].x, gc.map[i].y + gc.map[i].height })) {
            mouse_in_border = GrabbedBorder::LEFT_DOWN;
            i_rectangle = i;
        }

        if (check_if_mouse_in_point(
                mouse, Vector2{ gc.map[i].x + gc.map[i].width, gc.map[i].y })) {
            mouse_in_border = GrabbedBorder::RIGHT_UPPER;
            i_rectangle = i;
        }

        if (check_if_mouse_in_point(
                mouse,
                Vector2{ gc.map[i].x + gc.map[i].width,
                         gc.map[i].y + gc.map[i].height })) {
            mouse_in_border = GrabbedBorder::RIGHT_DOWN;
            i_rectangle = i;
        }

        if (check_if_mouse_in_point(
                mouse,
                Vector2{ gc.map[i].x + (gc.map[i].width / 2),
                         gc.map[i].y + (gc.map[i].height / 2) })) {
            mouse_in_border = GrabbedBorder::CENTER;
            i_rectangle = i;
        }
    }
}

void EditMode::check_if_mouse_in_horse(GameContext& gc, Vector2& mouse) {
    for (const auto& h : gc.horses) {
        if (CheckCollisionPointCircle(
                mouse, h->get_position(), h->get_radius()) &&
            IsMouseButtonDown(MOUSE_LEFT_BUTTON) &&
            mouse_in_border == GrabbedBorder::NONE && !mouse_in_uma) {
            std::println("Haz hecho click en {:}", h->get_name());
            mouse_in_uma = true;
            selected_uma = h.get();
        }
    }
}

void EditMode::delete_border(GameContext& gc, Vector2 mouse) {
    for (int i = 0; i < (int)gc.map.size(); i++) {
        if (check_if_mouse_in_point(
                mouse,
                Vector2{ gc.map[i].x + (gc.map[i].width / 2),
                         gc.map[i].y + (gc.map[i].height / 2) }) &&
            IsKeyPressed(KEY_D)) {
            std::println("Rectángulo {:} listo para ser borrado", i);
            gc.map.erase(gc.map.begin() + i);
        }
    }
}

unique_ptr<GameMode> EditMode::update(GameContext& gc) {
    UpdateMusicStream(gc.menu_song);
    Vector2 mouse = GetMousePosition();

    if (back_button_pressed) {
        std::println("INFO: Entering Menu Mode");
        return std::make_unique<MenuMode>();
    }

    if (IsKeyPressed(KEY_A)) {
        gc.map.emplace_back(Rectangle{ (GetScreenWidth() / 2.0F) - 50,
                                       (GetScreenHeight() / 2.0F) - 50,
                                       100,
                                       100 });
    }

    if (CheckCollisionPointCircle(mouse, gc.goal.position, 10) &&
        IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !mouse_in_uma &&
        mouse_in_border == GrabbedBorder::NONE) {
        mouse_in_goal = true;
    }

    if (mouse_in_goal) {
        gc.goal.position = mouse;

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            mouse_in_goal = false;
        }
    }

    delete_border(gc, mouse);

    check_if_mouse_in_horse(gc, mouse);
    check_if_mouse_in_border(gc, mouse);

    move_horse(mouse);
    move_border(gc, mouse);

    return nullptr;
}

void EditMode::render(GameContext& gc) {
    ClearBackground(RAYWHITE);
    for (auto b : gc.map) {
        DrawRectangleRec(b, PURPLE);
        DrawCircle(b.x, b.y, 5.0F, BLUE);
        DrawCircle(b.x + b.width, b.y, 5.0F, BLUE);
        DrawCircle(b.x, b.y + b.height, 5.0F, BLUE);
        DrawCircle(b.x + b.width, b.y + b.height, 5.0F, BLUE);
        DrawCircle(b.x + (b.width / 2), b.y + (b.height / 2), 5.0f, BLUE);
    };

    for (const auto& h : gc.horses) {
        h->render();
    }

    if (GuiButton(Rectangle{ 20, 20, 20, 20 }, "<")) {
        back_button_pressed = true;
    }

    DrawTextureEx(gc.goal.texture,
                  Vector2{ gc.goal.position.x - 10, gc.goal.position.y - 10 },
                  0.0F,
                  gc.goal.texture.width / 25000.0F,
                  WHITE);
}

std::unique_ptr<GameMode> PictureMode::update(GameContext& gc) {
    UpdateMusicStream(gc.menu_song);
    if (button_back_pressed) {
        return std::make_unique<MenuMode>();
    }

    for (int i = 0; i < 4; i++) {
        gc.horses[i]->portrait_render(
            Vector2{ 40, static_cast<float>(30.0 + (100.0 * i)) });
    }

    for (int i = 4; i < 8; i++) {
        gc.horses[i]->portrait_render(
            Vector2{ static_cast<float>(GetScreenWidth() / 2.0),
                     static_cast<float>(30.0 + (100.0 * (i - 4))) });
    }

    return nullptr;
}

void PictureMode::render(GameContext& gc) {
    ClearBackground(RAYWHITE);

    if (GuiButton(Rectangle{ 20, 20, 20, 20 }, "<")) {
        button_back_pressed = true;
    }
}
