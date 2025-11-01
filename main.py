import pyray as rl
from pyray import Vector2, Rectangle
from raylib import KEY_SPACE
from random import choice


class Horse:
    speed: Vector2

    def __init__(
        self, name: str, position: Vector2, radius: int, color: rl.Color
    ) -> None:
        self.name: str = name
        self.position: Vector2 = position
        self.radius: int = 20
        self.color: rl.Color = color

    def accelerate(self) -> None:
        self.position.x += self.speed.x
        self.position.y += self.speed.y

    def collide(self, b: Rectangle):
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
            self.speed.y *= -1.0
            if dy > 0:
                self.position.y = b.y + b.height + self.radius
            else:
                self.position.y = b.y - self.radius

    def collide_with_horse(self, h: "Horse") -> None:
        dx = self.position.x - h.position.x
        dy = self.position.y - h.position.y

        distance: float = (dx * dx + dy * dy) ** 0.5
        min_distance = self.radius + h.radius

        if distance < min_distance and distance > 0:
            # Normaliza el vector de colisión
            nx = dx / distance
            ny = dy / distance

            # Separa los círculos
            overlap = min_distance - distance
            self.position.x += nx * overlap
            self.position.y += ny * overlap

            # Refleja la velocidad del círculo 1
            # Producto punto: v · n
            dot = self.speed.x * nx + self.speed.y * ny

            # Nueva velocidad: v' = v - 2(v·n)n
            self.speed.x -= 2 * dot * nx
            self.speed.y -= 2 * dot * ny

            # Si el círculo 2 también se mueve (opcional)
            dot2 = h.speed.x * nx + h.speed.y * ny
            h.speed.x -= 2 * dot2 * nx
            h.speed.y -= 2 * dot2 * ny

    def check_collision_borders(self, bounds: list[rl.Rectangle]) -> None:
        for b in bounds:
            if rl.check_collision_circle_rec(self.position, self.radius, b):
                self.collide(b)

    def check_collision_horses(self, bounds: list["Horse"]) -> None:
        for b in bounds:
            if rl.check_collision_circles(
                self.position, self.radius, h.position, h.radius
            ):
                self.collide_with_horse(b)

    def render(self) -> None:
        rl.draw_circle_v(self.position, self.radius, self.color)
        rl.draw_text(
            self.name,
            int(self.position.x) - 15,
            int(self.position.y + self.radius / 2) + 10,
            8,
            rl.BLACK,
        )

    def start(self) -> None:
        self.speed = choice(
            [
                Vector2(5.0, 4.0),
                Vector2(-5.0, 4.0),
                Vector2(5.0, -4.0),
                Vector2(-5.0, -4.0),
            ],
        )


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
