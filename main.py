import pyray as rl
from pyray import Vector2, Rectangle
from raylib import KEY_SPACE


class Horse:
    def __init__(self, x: int | float, y: int | float, radius: int) -> None:
        self.position: Vector2 = Vector2(x, y)
        self.speed: Vector2 = Vector2(10.0, 8.0)
        self.radius: int = 20


def check_collision(horse: Horse, bounds: list[rl.Rectangle]):
    for b in bounds:
        collision = rl.check_collision_circle_rec(horse.position, horse.radius, b)

        if not collision:
            continue

        center_x = b.x + b.width / 2
        center_y = b.y + b.height / 2

        dx = horse.position.x - center_x
        dy = horse.position.y - center_y

        if abs(dx / b.width) > abs(dy / b.height):
            horse.speed.x *= -1.0
            if dx > 0:
                horse.position.x = b.x + b.width + horse.radius
            else:
                horse.position.x = b.x - horse.radius
        else:
            horse.speed.y *= -0.95
            if dy > 0:
                horse.position.y = b.y + b.height + horse.radius
            else:
                horse.position.y = b.y - horse.radius


WIDTH = 800
HEIGHT = 450

rl.set_config_flags(rl.ConfigFlags.FLAG_MSAA_4X_HINT)
rl.init_window(WIDTH, HEIGHT, "Hola")

special_week = Horse(rl.get_screen_width() / 2, rl.get_screen_height() / 2, 20)

all_bounds = [
    Rectangle(0, 0, 50, rl.get_screen_height()),
    Rectangle(rl.get_screen_width() - 50, 0, 50, rl.get_screen_height()),
    Rectangle(0, 0, rl.get_screen_width(), 50),
    Rectangle(0, rl.get_screen_height() - 50, rl.get_screen_width(), 50),
]

pause = False
frames_counter = 0

rl.set_target_fps(60)

while not rl.window_should_close():
    # Lógica del juego
    if rl.is_key_pressed(rl.KeyboardKey(KEY_SPACE)):
        pause = not pause

    if not pause:
        special_week.position.x += special_week.speed.x
        special_week.position.y += special_week.speed.y

        check_collision(special_week, all_bounds)
    else:
        frames_counter += 1
    # Fin de la lógica del juego

    # Renderización
    rl.begin_drawing()

    rl.clear_background(rl.WHITE)
    rl.draw_circle_v(special_week.position, special_week.radius, rl.MAROON)
    for b in all_bounds:
        rl.draw_rectangle_rec(b, rl.PURPLE)

    if pause and (frames_counter / 30) % 2:
        rl.draw_text("Paused", 350, 200, 30, rl.GRAY)

    rl.draw_fps(10, 10)

    rl.end_drawing()
    # Fin de renderización

rl.close_window()
