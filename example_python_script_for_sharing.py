from math import *
from PIL import Image, ImageDraw

def in_disc(point: complex, shift: int, rad: float):
    is_in = sqrt((point.real + shift) ** 2 + point.imag ** 2) <= rad
    return is_in


def turn(point: complex):
    global N1, N2, R1, R2, dot_list, draw, left_complex, right_complex
    point_being_rotated = point + 1
    for _ in range(N1 - 1):
        point_being_rotated *= left_complex
        point_being_rotated -= 1
        draw_point(point_being_rotated)
        if in_disc(point_being_rotated, 1, R1) and in_disc(point_being_rotated, -1, R2):
            check_if_visited(point_being_rotated)
        point_being_rotated += 1

    point_being_rotated = point - 1
    for _ in range(N2 - 1):
        point_being_rotated *= right_complex
        point_being_rotated -= -1
        draw_point(point_being_rotated)
        if in_disc(point_being_rotated, 1, R1) and in_disc(point_being_rotated, -1, R2):
            check_if_visited(point_being_rotated)
        point_being_rotated += -1


def draw_point(point: complex):
    global x_shift, y_shift, ppud, draw
    x = floor(point.real * ppud) + x_shift
    y = floor(point.imag * ppud) + y_shift
    draw.point((x, y), fill=(255, 255, 255))


def check_if_visited(point: complex):
    global to_be_rotated, dot_list
    x_string = str(int(point.real * 10 ** 8))
    y_string = str(int(point.imag * 10 ** 8))
    xy_string = x_string + " " + y_string
    if xy_string not in to_be_rotated.keys():
        to_be_rotated[xy_string] = 0
        dot_list.append(point)


# Simulation parameters. Play around with these!
phi = (1+sqrt(5))/2
R1, R2 = sqrt(3+phi), sqrt(3+phi)  # define the radii of the left and right disc
N1, N2 = 5, 5  # define the rotational symmetry of the left and right disc
ppud = 501  # shorthand for 'pixels per unit distance'
starting_point = complex(0, 0)  # define the point you want to have the symmetries act on

# create an image based on the simulation parameters and draw the circle boundaries. Don't change these.
x_size = floor((R1 + R2 + 2.5) * ppud) + 4
y_size = floor((2 * max(R1, R2) + 0.5) * ppud) + 4
x_shift = int(x_size / 2)
y_shift = int(y_size / 2)
im = Image.new(mode="RGB", size=(int(x_size), int(y_size)), color=(0, 0, 0))
draw = ImageDraw.Draw(im)
dot_list = [starting_point]  # this object is a 'queue' for points in the intersection that need to be rotated
to_be_rotated = {}  # this object is the ordered list which stores the hashes of the points in the intersection
left_complex = complex(cos(2 * pi / N1), sin(2 * pi / N1))
right_complex = complex(cos(2 * pi / N2), sin(2 * pi / N2))
draw_point(starting_point)
left_center = complex(-1, 0)
draw.ellipse((int(ppud * (left_center.real - R1)) + x_shift, int(ppud * (left_center.imag - R1)) + y_shift,
              int(ppud * (left_center.real + R1)) + x_shift, int(ppud * (left_center.imag + R1)) + y_shift),
             outline=(0, 50, 255))
right_center = complex(1, 0)
draw.ellipse((int(ppud * (right_center.real - R2)) + x_shift, int(ppud * (right_center.imag - R2)) + y_shift,
              int(ppud * (right_center.real + R2)) + x_shift, int(ppud * (right_center.imag + R2)) + y_shift),
             outline=(255, 50, 0))

# Main loop driving the simulation
count = 0
while len(dot_list) > 0 and count < 500000:
    point_being_checked = dot_list.pop(0)
    turn(point_being_checked)
    count += 1

# display the image
im.show()
