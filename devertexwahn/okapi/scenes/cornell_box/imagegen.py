import simpleimageio as sio
import figuregen
from figuregen.util import image

images = [
    sio.read("reference/cornell_box.naive_diffuse.spp1.embree.exr"),    #  1
    sio.read("reference/cornell_box.naive_diffuse.spp2.embree.exr"),    #  2
    sio.read("reference/cornell_box.naive_diffuse.spp4.embree.exr"),    #  3
    sio.read("reference/cornell_box.naive_diffuse.spp8.embree.exr"),    #  4
    sio.read("reference/cornell_box.naive_diffuse.spp16.embree.exr"),   #  5
    sio.read("reference/cornell_box.naive_diffuse.spp32.embree.exr"),   #  6
    sio.read("reference/cornell_box.naive_diffuse.spp64.embree.exr"),   #  7
    sio.read("reference/cornell_box.naive_diffuse.spp128.embree.exr"),  #  8
    sio.read("reference/cornell_box.naive_diffuse.spp256.embree.exr"),  #  9
    sio.read("reference/cornell_box.naive_diffuse.spp512.embree.exr"),  # 10
    sio.read("reference/cornell_box.naive_diffuse.spp1024.embree.exr"), # 11
    sio.read("reference/cornell_box.naive_diffuse.spp2048.embree.exr"), # 12
    sio.read("reference/cornell_box.naive_diffuse.spp4096.embree.exr"), # 13
    sio.read("reference/cornell_box.naive_diffuse.spp8192.embree.exr"), # 14

    sio.read("reference/cornell_box.naive_diffuse.spp1.octree.exr"),    #  1
    sio.read("reference/cornell_box.naive_diffuse.spp2.octree.exr"),    #  2
    sio.read("reference/cornell_box.naive_diffuse.spp4.octree.exr"),    #  3
    sio.read("reference/cornell_box.naive_diffuse.spp8.octree.exr"),    #  4
    sio.read("reference/cornell_box.naive_diffuse.spp16.octree.exr"),   #  5
    sio.read("reference/cornell_box.naive_diffuse.spp32.octree.exr"),   #  6
    sio.read("reference/cornell_box.naive_diffuse.spp64.octree.exr"),   #  7
    sio.read("reference/cornell_box.naive_diffuse.spp128.octree.exr"),  #  8
    sio.read("reference/cornell_box.naive_diffuse.spp256.octree.exr"),  #  9
    sio.read("reference/cornell_box.naive_diffuse.spp512.octree.exr"),  # 10
    sio.read("reference/cornell_box.naive_diffuse.spp1024.octree.exr"), # 11
    sio.read("reference/cornell_box.naive_diffuse.spp2048.octree.exr"), # 12
    sio.read("reference/cornell_box.naive_diffuse.spp4096.embree.exr"), # 13
    sio.read("reference/cornell_box.naive_diffuse.spp8192.embree.exr"), # 14
]

n_rows = 2
n_cols = 14
c_grid = figuregen.Grid(num_rows=n_rows, num_cols=n_cols)

# fill grid with image data
for row in range(n_rows):
    for col in range(n_cols):
        c_grid.get_element(row,col).set_image(figuregen.PNG(image.lin_to_srgb(images[col+n_cols*row])))

figuregen.horizontal_figure([c_grid], width_cm=18., filename='cornell_box.pdf')
#figuregen.horizontal_figure([c_grid], width_cm=18., filename='cornell_box.html')
