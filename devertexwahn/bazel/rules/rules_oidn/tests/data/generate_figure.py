import simpleimageio as sio
import figuregen
from figuregen.util import image

images = [
    sio.read("cornell_box.naive_diffuse.box_filter.spp1.embree.exr"),    #  1
    sio.read("cornell_box.naive_diffuse.box_filter.spp2.embree.exr"),    #  2
    sio.read("cornell_box.naive_diffuse.box_filter.spp4.embree.exr"),    #  3
    sio.read("cornell_box.naive_diffuse.box_filter.spp8.embree.exr"),    #  4
    sio.read("cornell_box.naive_diffuse.box_filter.spp16.embree.exr"),   #  5
    sio.read("cornell_box.naive_diffuse.box_filter.spp32.embree.exr"),   #  6
    sio.read("cornell_box.naive_diffuse.box_filter.spp64.embree.exr"),   #  7
    sio.read("cornell_box.naive_diffuse.box_filter.spp128.embree.exr"),  #  8
    sio.read("cornell_box.naive_diffuse.box_filter.spp256.embree.exr"),  #  9
    sio.read("cornell_box.naive_diffuse.box_filter.spp512.embree.exr"),  # 10
    sio.read("cornell_box.naive_diffuse.box_filter.spp1024.embree.exr"), # 11

    sio.read("denoised_cornell_box.naive_diffuse.box_filter.spp1.embree.exr"),    #  1
    sio.read("denoised_cornell_box.naive_diffuse.box_filter.spp2.embree.exr"),    #  2
    sio.read("denoised_cornell_box.naive_diffuse.box_filter.spp4.embree.exr"),    #  3
    sio.read("denoised_cornell_box.naive_diffuse.box_filter.spp8.embree.exr"),    #  4
    sio.read("denoised_cornell_box.naive_diffuse.box_filter.spp16.embree.exr"),   #  5
    sio.read("denoised_cornell_box.naive_diffuse.box_filter.spp32.embree.exr"),   #  6
    sio.read("denoised_cornell_box.naive_diffuse.box_filter.spp64.embree.exr"),   #  7
    sio.read("denoised_cornell_box.naive_diffuse.box_filter.spp128.embree.exr"),  #  8
    sio.read("denoised_cornell_box.naive_diffuse.box_filter.spp256.embree.exr"),  #  9
    sio.read("denoised_cornell_box.naive_diffuse.box_filter.spp512.embree.exr"),  # 10
    sio.read("denoised_cornell_box.naive_diffuse.box_filter.spp1024.embree.exr"), # 11

    sio.read("cornell_box.naive_diffuse.gaussian_filter.spp1.embree.exr"),    #  1
    sio.read("cornell_box.naive_diffuse.gaussian_filter.spp2.embree.exr"),    #  2
    sio.read("cornell_box.naive_diffuse.gaussian_filter.spp4.embree.exr"),    #  3
    sio.read("cornell_box.naive_diffuse.gaussian_filter.spp8.embree.exr"),    #  4
    sio.read("cornell_box.naive_diffuse.gaussian_filter.spp16.embree.exr"),   #  5
    sio.read("cornell_box.naive_diffuse.gaussian_filter.spp32.embree.exr"),   #  6
    sio.read("cornell_box.naive_diffuse.gaussian_filter.spp64.embree.exr"),   #  7
    sio.read("cornell_box.naive_diffuse.gaussian_filter.spp128.embree.exr"),  #  8
    sio.read("cornell_box.naive_diffuse.gaussian_filter.spp256.embree.exr"),  #  9
    sio.read("cornell_box.naive_diffuse.gaussian_filter.spp512.embree.exr"),  # 10
    sio.read("cornell_box.naive_diffuse.gaussian_filter.spp1024.embree.exr"), # 11


    sio.read("denoised_cornell_box.naive_diffuse.gaussian_filter.spp1.embree.exr"),    #  1
    sio.read("denoised_cornell_box.naive_diffuse.gaussian_filter.spp2.embree.exr"),    #  2
    sio.read("denoised_cornell_box.naive_diffuse.gaussian_filter.spp4.embree.exr"),    #  3
    sio.read("denoised_cornell_box.naive_diffuse.gaussian_filter.spp8.embree.exr"),    #  4
    sio.read("denoised_cornell_box.naive_diffuse.gaussian_filter.spp16.embree.exr"),   #  5
    sio.read("denoised_cornell_box.naive_diffuse.gaussian_filter.spp32.embree.exr"),   #  6
    sio.read("denoised_cornell_box.naive_diffuse.gaussian_filter.spp64.embree.exr"),   #  7
    sio.read("denoised_cornell_box.naive_diffuse.gaussian_filter.spp128.embree.exr"),  #  8
    sio.read("denoised_cornell_box.naive_diffuse.gaussian_filter.spp256.embree.exr"),  #  9
    sio.read("denoised_cornell_box.naive_diffuse.gaussian_filter.spp512.embree.exr"),  # 10
    sio.read("denoised_cornell_box.naive_diffuse.gaussian_filter.spp1024.embree.exr"), # 11
]

n_rows = 4
n_cols = 11
c_grid = figuregen.Grid(num_rows=n_rows, num_cols=n_cols)

# fill grid with image data
for row in range(n_rows):
    for col in range(n_cols):
        c_grid.get_element(row,col).set_image(figuregen.PNG(image.lin_to_srgb(images[col+n_cols*row])))

figuregen.horizontal_figure([c_grid], width_cm=18., filename='cornell_box.pdf')
#figuregen.horizontal_figure([c_grid], width_cm=18., filename='cornell_box.html')
