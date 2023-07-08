from math import *
from PIL import Image, ImageDraw, ImageOps
from time import *
import colorsys
import re
import cmath

def define_image_size(rad: float, ppud: int):

    val = rad + 0.2
    x_min, y_min, x_max, y_max = -val, -val, val, val
    x_span = x_max - x_min
    y_span = y_max - y_min

    x_width = 4 + int(x_span * ppud)
    y_width = 4 + int(y_span * ppud)

    return x_width, y_width

def get_roots_of_unity(N):
    roots = []
    rou = complex(1, 0)
    roots.append(rou)
    rotation = complex(cos(2*pi/N), sin(2*pi/N))
    for _ in range(N-1):
        rou *= rotation
        roots.append(rou)

    return roots

def color_storage():
    global plate_color, boundary_color, rou_color, point_color, original_line_color
    plate_color = (0, 0, 0)
    boundary_color = (200, 200, 200)
    rou_color = (200, 100, 0)
    point_color = (0, 100, 200)
    original_line_color = (255, 255, 100)

def draw_base_circle():
    global draw, rad, ppud, x_size, y_size, boundary_color

    x_min = int(x_size / 2 - rad * ppud)
    y_min = int(y_size / 2 - rad * ppud)
    x_max = int(x_size / 2 + rad * ppud)
    y_max = int(y_size / 2 + rad * ppud)

    draw.ellipse((x_min, y_min, x_max, y_max), width=2, outline=boundary_color)

def draw_rous(root_rad: int, render: ImageDraw.Draw):
    global rous, ppud, rou

    for root in rous:
        xt = root.real
        yt = root.imag

        x_min = int(x_size / 2 + xt * ppud) - root_rad
        y_min = int(y_size / 2 + yt * ppud) - root_rad
        x_max = int(x_size / 2 + xt * ppud) + root_rad
        y_max = int(y_size / 2 + yt * ppud) + root_rad

        render.ellipse((x_min, y_min, x_max, y_max), outline=rou_color, fill=rou_color)

def draw_point(point_in: complex, root_rad: int, render: ImageDraw.Draw):
    global point_color

    xt = point_in.real
    yt = point_in.imag
    # # filled in circles
    # x_min = int(x_size / 2 + xt * ppud) - root_rad
    # y_min = int(y_size / 2 + yt * ppud) - root_rad
    # x_max = int(x_size / 2 + xt * ppud) + root_rad
    # y_max = int(y_size / 2 + yt * ppud) + root_rad
    # render.ellipse((x_min, y_min, x_max, y_max), outline=color, fill=color)

    # single pixel
    x = int(x_size / 2 + xt * ppud)
    y = int(y_size / 2 + yt * ppud)

    render.point((x, y), fill=point_color)


def draw_original_line(render: ImageDraw.Draw):
    global original_line_color, original_line, ppud, original_line_rad

    for point in original_line:
        xt = point.real
        yt = point.imag
        x = int(x_size / 2 + xt * ppud)
        y = int(y_size / 2 + yt * ppud)

        x_min = int(x_size / 2 + xt * ppud) - original_line_rad
        y_min = int(y_size / 2 + yt * ppud) - original_line_rad
        x_max = int(x_size / 2 + xt * ppud) + original_line_rad
        y_max = int(y_size / 2 + yt * ppud) + original_line_rad
        render.ellipse((x_min, y_min, x_max, y_max), outline=original_line_color, fill=original_line_color)

        # render.point((x, y), fill=original_line_color)

def mirror_point_through_point(point_moving: complex, mirror_point: complex):
    diff = mirror_point - point_moving
    final = point_moving + 2*diff
    return final

def move_circle(move_to):
    global circle_position, N, rous
    diff = (N + (move_to - circle_position)) % N
    circle_position = move_to

def move_point_outside_circ(point: complex, move_to: int):
    global circle_position, N, rous
    diff = (N + (move_to - circle_position)) % N
    multiplier = rous[diff]
    point *= multiplier
    return point

def string_to_list(input_string: str):
    sanitized_string = re.sub('[^0-9,]', '', input_string)
    str_numbers = sanitized_string.split(',')
    int_numbers = [int(number) for number in str_numbers if number]
    return int_numbers

def in_disc(p_in: complex):
    global rad

    if sqrt(p_in.real**2 + p_in.imag**2) <= rad:
        return True
    return False

def get_seed_points():
    global rad, N, ppud

    return_list = []

    # span = 2*rad
    # div = span/1000
    # loc = -rad
    # while loc <= 2+rad-div:
    #     return_list.append(complex(loc, 0))
    #     loc += div

    rise = -sin(6*pi/N)
    run = 1-cos(6*pi/N)
    slope = rise/run

    span = 2*rad
    div = span/(4*ppud)
    loc = 1
    return_list.append(complex(1, 0))
    while loc <= rad:
        loc += div
        temp = complex(loc, (loc-1)*slope)
        if in_disc(temp):
            return_list.append(temp)

    loc = 1
    while loc >= -rad:
        loc -= div
        temp = complex(loc, (loc - 1) * slope)
        if in_disc(temp):
            return_list.append(temp)


    return return_list

def run_sequence(sequence_in):
    global point_list

    for pos in string_to_list(sequence_in):
        storage = []
        for i in range(len(point_list)):
            point = point_list.pop()
            if not in_disc(point):
                point = move_point_outside_circ(point, pos)
            storage.append(point)
        point_list.clear()
        for item in storage:
            point_list.append(item)
        storage.clear()
        move_circle(pos)

        storage = []
        for i in range(len(point_list)):
            point = point_list.pop()
            point = mirror_point_through_point(point, rous[pos])
            if in_disc(point):
                draw_point(point, dot_rad, draw)
            storage.append(point)
        point_list.clear()
        for item in storage:
            point_list.append(item)
        storage.clear()


# simulation params
N = 8
# rad = sqrt(3-2*cos(4*pi/N)) # General upper bound
# rad = sqrt(2*(20-11*sqrt(3))) # N = 12
rad = sqrt(5*(2-sqrt(2))) # N = 8
# rad = 1.6235740983 # N = 7 estimate
ppud = 600



if True:
    # initialize sim
    x_size, y_size = define_image_size(rad, ppud)
    rous = get_roots_of_unity(N)
    circle_position = 0
    point_list = get_seed_points()


    # initialize image
    color_storage()
    im = Image.new(mode="RGB", size=(x_size, y_size), color=plate_color)
    draw = ImageDraw.Draw(im)
    dot_rad = int(ppud/100)
    original_line_rad = int(ppud/800)
    draw_base_circle()
    # draw_rous(dot_rad, draw)

    original_line = []
    for point in point_list:
        draw_point(point, 1, draw)
        original_line.append(point)

    sequence1 = '1,2,1,2,3,2,1,0'
    sequence2 = '1,2,3,2,1,2,1,0'
    sequence3 = '1,2,3,2,1,0'

    for k in range(150):
        run_sequence(sequence1)
        run_sequence(sequence2)
        run_sequence(sequence3)

    for point in point_list:
        if in_disc(point):
            draw_point(point, 1, draw)

    draw_original_line(draw)
    draw_rous(dot_rad, draw)

    # show the image
    im = ImageOps.flip(im)

    filename = 'alt_reference_frame/images/N=%s, tc = %s.png' % (N, round(time()))
    im.save(filename)
    im.show()













