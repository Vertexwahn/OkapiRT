import simpleimageio as sio
import figuregen
from figuregen.util import image

images = [
    sio.read("ajax.ao.spp1.embree.exr"),    #  1
    sio.read("ajax.ao.spp2.embree.exr"),    #  2
    sio.read("ajax.ao.spp4.embree.exr"),    #  3
    sio.read("ajax.ao.spp8.embree.exr"),    #  4
    sio.read("ajax.ao.spp16.embree.exr"),   #  5
    sio.read("ajax.ao.spp32.embree.exr"),   #  6
    sio.read("ajax.ao.spp64.embree.exr"),   #  7
    sio.read("ajax.ao.spp128.embree.exr"),  #  8
    sio.read("ajax.ao.spp256.embree.exr"),  #  9
    sio.read("ajax.ao.spp512.embree.exr"),  # 10
    sio.read("ajax.ao.spp1024.embree.exr"), # 11
    sio.read("ajax.ao.spp2048.embree.exr"), # 12
    sio.read("ajax.ao.spp4096.embree.exr"), # 13
    sio.read("ajax.ao.spp8192.embree.exr"), # 14

    sio.read("ajax.ao.spp1.octree.exr"),    #  1
    sio.read("ajax.ao.spp2.octree.exr"),    #  2
    sio.read("ajax.ao.spp4.octree.exr"),    #  3
    sio.read("ajax.ao.spp8.octree.exr"),    #  4
    sio.read("ajax.ao.spp16.octree.exr"),   #  5
    sio.read("ajax.ao.spp32.octree.exr"),   #  6
    sio.read("ajax.ao.spp64.octree.exr"),   #  7
    sio.read("ajax.ao.spp128.octree.exr"),  #  8
    sio.read("ajax.ao.spp256.octree.exr"),  #  9
    sio.read("ajax.ao.spp512.octree.exr"),  # 10
    sio.read("ajax.ao.spp1024.octree.exr"), # 11
    sio.read("ajax.ao.spp2048.octree.exr"), # 12
    sio.read("ajax.ao.spp4096.octree.exr"), # 13
    sio.read("ajax.ao.spp8192.octree.exr"), # 14
]

n_rows = 2
n_cols = 14
c_grid = figuregen.Grid(num_rows=n_rows, num_cols=n_cols)

# fill grid with image data
for row in range(n_rows):
    for col in range(n_cols):
        c_grid.get_element(row,col).set_image(figuregen.PNG(image.lin_to_srgb(images[col+n_cols*row])))

figuregen.horizontal_figure([c_grid], width_cm=18., filename='ajax.pdf')
#figuregen.horizontal_figure([c_grid], width_cm=18., filename='ajax.html')
