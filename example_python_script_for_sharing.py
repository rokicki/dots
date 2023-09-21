from math import *
from PIL import Image, ImageDraw

def in_disc(point_in: complex, shift_in: int, rad_in: float):
    point_in += shift_in
    if sqrt(point_in.real ** 2 + point_in.imag ** 2) <= rad_in:
        return True
    return False


def turn(point_in: complex):
    global N1, N2, R1, R2, dot_list, draw
    temp = point_in + 1
    for _ in range(N1 - 1):
        temp *= complex(cos(2 * pi / N1), sin(2 * pi / N1))
        temp -= 1
        draw_point(temp)
        if in_disc(temp, 1, R1) and in_disc(temp, -1, R2):
            hash_check(temp)
        temp += 1

    temp = point_in - 1
    for _ in range(N2 - 1):
        temp *= complex(cos(2 * pi / N2), sin(2 * pi / N2))
        temp -= -1
        draw_point(temp)
        if in_disc(temp, 1, R1) and in_disc(temp, -1, R2):
            hash_check(temp)
        temp += -1


def draw_point(point_in: complex):
    global x_shift, y_shift, ppud, draw
    x = floor(point_in.real * ppud) + x_shift
    y = floor(point_in.imag * ppud) + y_shift
    draw.point((x, y), fill=(255, 255, 255))


def hash_check(point_in: complex):
    global intersection_dictionary, dot_list
    x_string = str(int(point_in.real * 10 ** 8))
    y_string = str(int(point_in.imag * 10 ** 8))
    hashed = hash(x_string + " " + y_string)
    if hashed not in intersection_dictionary.keys():
        intersection_dictionary[hashed] = 0
        dot_list.append(point_in)

# Simulation parameters. Play around with these
R1, R2 = 1.623, 1.623  # define the radii of the left and right disc
N1, N2 = 7, 7  # define the rotational symmetry of the left and right disc
ppud = 700  # shorthand for 'pixels per unit distance'
starting_point = complex(0, 0)  # define the point you want to have the symmetries act on


# create an image based on the simulation parameters and draw the circle boundaries
x_size = floor((R1 + R2 + 2.5) * ppud) + 4
y_size = floor((2 * max(R1, R2) + 0.5) * ppud) + 4
x_shift = int(x_size / 2)
y_shift = int(y_size / 2)

im = Image.new(mode="RGB", size=(int(x_size), int(y_size)), color=(0, 0, 0))
draw = ImageDraw.Draw(im)
dot_list = [starting_point]  # this object is a 'queue' for points in the intersection that need to be rotated
intersection_dictionary = {}  # this object is the ordered list which stores the hashes of the points in the intersection
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
while len(dot_list) > 0 and count < 50000:
    point_being_checked = dot_list.pop(0)
    turn(point_being_checked)
    count += 1

# display the image
im.show()
