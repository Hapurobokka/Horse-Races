from dataclasses import dataclass
import pyray as rl
from raylib import KEY_SPACE
from pyray import Vector2, Rectangle 
from horse import Horse


class GameContext:
    def __init__(self) -> None:
        self.pause: bool = False
        self.frames_counter: int = 0
        self.race_started: bool = False
        self.start_countdown: bool = False
        self.count_down: float = 0
        self.victory: bool = False
        self.winner: str = ""


@dataclass
class Goal:
    position: Vector2
    radius: int


def pause_game(gc: GameContext) -> None:
    if not gc.race_started:
        gc.start_countdown = True
    else:
        gc.pause = not gc.pause


def start_game_countdown(gc: GameContext):
    if gc.count_down >= 2:
        gc.race_started = True
        gc.start_countdown = False
    else:
        gc.count_down += rl.get_frame_time()


def check_victory(h: Horse, g: Goal, gc: GameContext):
    if rl.check_collision_circles(h.position, h.radius, g.position, g.radius):
        gc.victory = True
        gc.winner = h.name


WIDTH = 800
HEIGHT = 450

rl.set_config_flags(rl.ConfigFlags.FLAG_MSAA_4X_HINT)
rl.init_window(WIDTH, HEIGHT, "Umamusume")

all_horses = [
    Horse(
        "SPCWK",
        Vector2(200, 250),
        20,
        rl.PINK,
    ),
    Horse(
        "SILSUZ",
        Vector2(200, 300),
        50,
        rl.GREEN,
    ),
    Horse(
        "TOTE",
        Vector2(250, 300),
        50,
        rl.PURPLE,
    ),
    Horse(
        "GLSP",
        Vector2(250, 250),
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
    Rectangle(rl.get_screen_width() - 350, 0, 175, 120),
]

goal = Goal(Vector2(700, 350), 10)

for h in all_horses:
    h.start()

rl.set_target_fps(60)

gc = GameContext()

while not rl.window_should_close():
    # Lógica del juego

    # Cosas que detienen el juego
    if rl.is_key_pressed(rl.KeyboardKey(KEY_SPACE)):
        pause_game(gc)

    if gc.start_countdown:
        start_game_countdown(gc)

    # Simulación de físicas (?)
    if gc.winner:
        gc.frames_counter += 1
    elif gc.race_started and not gc.pause:
        for h in all_horses:
            h.check_collision_borders(all_bounds)
            h.accelerate()
            h.check_collision_horses(all_horses)
            check_victory(h, goal, gc)
    else:
        gc.frames_counter += 1

    # Fin de la lógica del juego

    # Renderización
    rl.begin_drawing()

    rl.clear_background(rl.WHITE)
    for b in all_bounds:
        rl.draw_rectangle_rec(b, rl.PURPLE)

    for h in all_horses:
        h.render()
    rl.draw_circle_v(goal.position, goal.radius, rl.ORANGE)

    if gc.pause and (gc.frames_counter / 30) % 2:
        rl.draw_text("Paused", 350, 200, 30, rl.GRAY)

    if not gc.start_countdown and not gc.race_started:
        rl.draw_text("Ready?", 320, 200, 60, rl.YELLOW)

    if gc.start_countdown:
        rl.draw_text(f"{gc.count_down + 1:.0f}", 320, 200, 60, rl.RED)

    if gc.victory and (gc.frames_counter / 30) % 2:
        rl.draw_text(f"WINNER: {gc.winner}", 350, 200, 30, rl.YELLOW)

    rl.draw_fps(10, 10)

    rl.end_drawing()
    # Fin de renderización

rl.close_window()
