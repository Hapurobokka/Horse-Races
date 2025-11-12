from pyray import Vector2, Rectangle
import pyray as rl


class Horse:
    def __init__(
        self, name: str, color: rl.Color, boop: rl.Sound, texture: str
    ) -> None:
        self.name: str = name
        self.position: Vector2 = Vector2(0, 0)
        self.radius: int = 20
        self.color: rl.Color = color
        self.speed: Vector2 = Vector2(0, 0)
        self.boop: rl.Sound = boop
        self.texture: rl.Texture = rl.load_texture(texture)

    def accelerate(self) -> None:
        self.position.x += self.speed.x
        self.position.y += self.speed.y

    def collide_with_border(self, b: Rectangle):
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

        # type: ignore
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
                self.collide_with_border(b)
                rl.play_sound(self.boop)

    def check_collision_horses(self, bounds: list["Horse"]) -> None:
        for h in bounds:
            if rl.check_collision_circles(
                self.position, self.radius, h.position, h.radius
            ):
                self.collide_with_horse(h)

    def render(self) -> None:
        rl.draw_texture_ex(
            self.texture,
            Vector2(
                self.position.x - self.radius,
                self.position.y - self.radius,
            ),
            0,
            self.texture.width / 6000.0,
            rl.WHITE,
        )

        rl.draw_text(
            self.name,
            int(self.position.x) - 15,
            int(self.position.y + self.radius / 2) + 10,
            8,
            rl.BLACK,
        )
