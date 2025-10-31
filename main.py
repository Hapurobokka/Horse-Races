import pyray as rl
from raylib import KEY_SPACE

WIDTH = 800
HEIGHT = 450

rl.set_config_flags(rl.ConfigFlags.FLAG_MSAA_4X_HINT)
rl.init_window(WIDTH, HEIGHT, "Hola")

ball_position = rl.Vector2(rl.get_screen_width() / 2, rl.get_screen_height() / 2)
ball_speed = rl.Vector2(10.0, 8.0)
ball_radius = 20

left_bound_rec = rl.Rectangle(0, 0, 50, rl.get_screen_height())
right_bound_rec = rl.Rectangle(
    rl.get_screen_width() - 50, 0, 50, rl.get_screen_height()
)
upper_bound_rec = rl.Rectangle(0, 0, rl.get_screen_width(), 50)
lower_bound_rec = rl.Rectangle(
    0, rl.get_screen_height() - 50, rl.get_screen_width(), 50
)

upper_bounds = [
    upper_bound_rec,
    lower_bound_rec,
]

side_bounds = [
    left_bound_rec,
    right_bound_rec,
]

pause = False
frames_counter = 0

rl.set_target_fps(60)

while not rl.window_should_close():
    if rl.is_key_pressed(rl.KeyboardKey(KEY_SPACE)):
        pause = not pause

    if not pause:
        ball_position.x += ball_speed.x
        ball_position.y += ball_speed.y

        for b in side_bounds:
            collision: bool = rl.check_collision_circle_rec(ball_position, ball_radius, b)
            if collision:
                center_x = b.x + b.width / 2
                center_y = b.y + b.height / 2

        for b in side_bounds:
            collision = rl.check_collision_circle_rec(ball_position, ball_radius, b)
            if collision:
                ball_speed.x *= -1.0
                if ball_position.x < b.x + b.width / 2:
                    ball_position.x = b.x - ball_radius
                else:
                    ball_position.x = b.x + b.width + ball_radius

        for b in upper_bounds:
            collision = rl.check_collision_circle_rec(ball_position, ball_radius, b)
            if collision:
                ball_speed.y *= -0.95
                if ball_position.y < b.y + b.height / 2:
                    ball_position.y = b.y - ball_radius
                else:
                    ball_position.y = b.y + b.height + ball_radius

    else:
        frames_counter += 1

    # Renderización
    rl.begin_drawing()

    rl.clear_background(rl.WHITE)
    rl.draw_circle_v(ball_position, ball_radius, rl.MAROON)
    rl.draw_rectangle_rec(left_bound_rec, rl.PURPLE)
    rl.draw_rectangle_rec(right_bound_rec, rl.PURPLE)
    rl.draw_rectangle_rec(upper_bound_rec, rl.PURPLE)
    rl.draw_rectangle_rec(lower_bound_rec, rl.PURPLE)

    if pause and (frames_counter / 30) % 2:
        rl.draw_text("Paused", 350, 200, 30, rl.GRAY)

    rl.draw_fps(10, 10)

    rl.end_drawing()
    # Fin de renderización

rl.close_window()
