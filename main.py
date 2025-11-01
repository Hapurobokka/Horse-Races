import pyray as rl
from pyray import Vector2, Rectangle
from raylib import KEY_SPACE


class Horse:
    def __init__(self, name: str, x: int | float, y: int | float, radius: int) -> None:
        self.name: str = name
        self.position: Vector2 = Vector2(x, y)
        self.speed: Vector2 = Vector2(10.0, 8.0)
        self.radius: int = 20

    def accelerate(self) -> None:
        self.position.x += self.speed.x
        self.position.y += self.speed.y

    def check_collision(self, bounds: list[rl.Rectangle]) -> None:
        for b in bounds:
            collision = rl.check_collision_circle_rec(self.position, self.radius, b)

            if not collision:
                continue

            center_x = b.x + b.width / 2
            center_y = b.y + b.height / 2

            dx = self.position.x - center_x
            dy = self.position.y - center_y

            if abs(dx / b.width) > abs(dy / b.height):
                self.speed.x *= -1.0
                if dx > 0:
                    self.position.x = b.x + b.width + self.radius
                else:
                    self.position.x = b.x - self.radius
            else:
                self.speed.y *= -0.95
                if dy > 0:
                    self.position.y = b.y + b.height + self.radius
                else:
                    self.position.y = b.y - self.radius


WIDTH = 800
HEIGHT = 450

rl.set_config_flags(rl.ConfigFlags.FLAG_MSAA_4X_HINT)
rl.init_window(WIDTH, HEIGHT, "Hola")

special_week = Horse(
    "Special Week", rl.get_screen_width() / 2, rl.get_screen_height() / 2, 20
)

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
        special_week.accelerate()
        special_week.check_collision(all_bounds)
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
