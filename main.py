from dataclasses import dataclass
import pyray as rl
from raylib import KEY_SPACE
from pyray import (
    Vector2,
    Rectangle,
    get_screen_height,
    get_screen_width,
)
from horse import Horse
from random import shuffle, choice


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


def start_game_countdown(gc: GameContext, m: rl.Music):
    if gc.count_down >= 2:
        gc.race_started = True
        gc.start_countdown = False
        rl.play_music_stream(m)
    else:
        gc.count_down += rl.get_frame_time()


def check_victory(h: Horse, g: Goal, gc: GameContext):
    if rl.check_collision_circles(h.position, h.radius, g.position, g.radius):
        gc.victory = True
        gc.winner = h.name


def randomize_race(horses: list[Horse]) -> None:
    starting_positions = [
        Vector2(80, 50),
        Vector2(140, 50),
        Vector2(80, 100),
        Vector2(140, 100),
        Vector2(80, 150),
        Vector2(140, 150),
        Vector2(80, 200),
        Vector2(140, 200),
    ]
    shuffle(starting_positions)
    print(starting_positions)
    for h in horses:
        h.position = starting_positions.pop()
        h.speed = choice(
            [
                Vector2(2.0, 1.0),
                Vector2(-2.0, 1.0),
                Vector2(2.0, -1.0),
                Vector2(-2.0, -1.0),
                Vector2(1.0, 2.0),
                Vector2(-1.0, 2.0),
                Vector2(1.0, -2.0),
                Vector2(-1.0, -2.0),
                Vector2(1.5, 1.5),
                Vector2(-1.5, 1.5),
                Vector2(1.5, -1.5),
                Vector2(-1.5, -1.5),
            ],
        )


WIDTH = 800
HEIGHT = 450

rl.set_config_flags(rl.ConfigFlags.FLAG_MSAA_4X_HINT)
rl.init_window(WIDTH, HEIGHT, "Umamusume")

rl.init_audio_device()

boop = rl.load_sound("collide.wav")
rl.set_sound_volume(boop, 0.1)

ost = rl.load_music_stream("versus.mp3")
ost.looping = True
rl.set_music_volume(ost, 0.4)

victory = rl.load_music_stream("victory.mp3")
victory.looping = True
rl.set_music_volume(victory, 1.0)

all_horses = [
    Horse("SPCWK", rl.PINK, boop),
    Horse("SILSUZ", rl.GREEN, boop),
    Horse("TOTE", rl.MAGENTA, boop),
    Horse("GLSP", rl.YELLOW, boop),
    Horse("TWTB", rl.BLUE, boop),
    Horse("SILOV", rl.RED, boop),
    Horse("MTKFKKTR", rl.SKYBLUE, boop),
    Horse("MJMQ", rl.PURPLE, boop),
]

all_bounds = [
    Rectangle(0, 0, get_screen_width() - 20, 20),
    Rectangle(0, 20, 20, get_screen_height() - 20),
    Rectangle(0, get_screen_height() - 20, get_screen_width() - 20, 20),
    Rectangle(get_screen_width() - 20, 0, 20, get_screen_height()),
    Rectangle(200, 20, 60, 200),
    Rectangle(340, 20, 60, 240),
    Rectangle(440, 20, 60, 40),
    Rectangle(540, 20, 140, 60),
    Rectangle(480, 140, 300, 40),
    Rectangle(20, get_screen_height() - 180, 100, 40),
    Rectangle(460, get_screen_height() - 180, 240, 40),
    Rectangle(20, get_screen_height() - 100, 100, 80),
    Rectangle(200, get_screen_height() - 100, 60, 80),
    Rectangle(340, get_screen_height() - 60, 60, 40),
    Rectangle(460, get_screen_height() - 60, 240, 40),
]

goal = Goal(Vector2(get_screen_width() - 40, 40), 10)

randomize_race(all_horses)

rl.set_target_fps(60)

gc = GameContext()

while not rl.window_should_close():
    rl.update_music_stream(ost)
    rl.update_music_stream(victory)
    # Lógica del juego

    # Cosas que detienen el juego
    if rl.is_key_pressed(rl.KeyboardKey(KEY_SPACE)):
        pause_game(gc)

    if gc.start_countdown:
        start_game_countdown(gc, ost)

    if gc.winner:
        # Desmadre para reproducir música
        if rl.is_music_stream_playing(ost):
            rl.stop_music_stream(ost)
        if not rl.is_music_stream_playing(victory):
            rl.play_music_stream(victory)
        gc.frames_counter += 1

    elif gc.race_started and not gc.pause:
        for h in all_horses:
            # Simulación de físicas (?)
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

rl.unload_sound(boop)
rl.unload_music_stream(ost)
rl.unload_music_stream(victory)
rl.close_audio_device()
rl.close_window()
