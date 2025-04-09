import mitsuba as mi

mi.set_variant('scalar_rgb')

scene = mi.load_file("cornell_box.aov.integrator.spp1.mitsuba.xml")

ssp = 1024

img = mi.render(scene, spp=ssp)

mi.Bitmap(img).write("cornell_box.aov.integrator.spp" + str(ssp) + ".aov.mitsuba.exr")
