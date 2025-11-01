import pyray as rl
from raylib import KEY_SPACE
from pyray import Vector2, Rectangle
from horse import Horse


WIDTH = 800
HEIGHT = 450

rl.set_config_flags(rl.ConfigFlags.FLAG_MSAA_4X_HINT)
rl.init_window(WIDTH, HEIGHT, "Hola")

all_horses = [
    Horse(
        "SPCWK",
        Vector2(rl.get_screen_width() / 2, rl.get_screen_height() / 2),
        20,
        rl.PINK,
    ),
    Horse(
        "SILSUZ",
        Vector2((rl.get_screen_width() / 2) + 30, rl.get_screen_height() / 2),
        50,
        rl.GREEN,
    ),
    Horse(
        "TOTE",
        Vector2((rl.get_screen_width() / 2) + 50, rl.get_screen_height() / 2),
        50,
        rl.PURPLE,
    ),
    Horse(
        "GLSP",
        Vector2(rl.get_screen_width() / 2, (rl.get_screen_height() / 2) + 50),
        50,
        rl.YELLOW,
    ),
]

all_bounds = [
    Rectangle(0, 0, 50, rl.get_screen_height()),
    Rectangle(rl.get_screen_width() - 50, 0, 50, rl.get_screen_height()),
    Rectangle(0, 0, rl.get_screen_width(), 50),
    Rectangle(0, rl.get_screen_height() - 50, rl.get_screen_width(), 50),
    Rectangle(150, 0, 175, 200),
    Rectangle(rl.get_screen_width() - 350, 200, 175, 300),
]

for h in all_horses:
    h.start()

pause = False
frames_counter = 0

rl.set_target_fps(60)

while not rl.window_should_close():
    # Lógica del juego
    if rl.is_key_pressed(rl.KeyboardKey(KEY_SPACE)):
        pause = not pause

    if not pause:
        for h in all_horses:
            h.check_collision_borders(all_bounds)
            h.check_collision_horses(all_horses)
            h.accelerate()
    else:
        frames_counter += 1
    # Fin de la lógica del juego

    # Renderización
    rl.begin_drawing()

    rl.clear_background(rl.WHITE)
    for h in all_horses:
        h.render()
    for b in all_bounds:
        rl.draw_rectangle_rec(b, rl.PURPLE)

    if pause and (frames_counter / 30) % 2:
        rl.draw_text("Paused", 350, 200, 30, rl.GRAY)

    rl.draw_fps(10, 10)

    rl.end_drawing()
    # Fin de renderización

rl.close_window()
