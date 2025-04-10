import mitsuba as mi

mi.set_variant('scalar_spectral')

scene = mi.load_file("mitsuba-cbox/cbox-spectral.xml")

ssp = 1024

img = mi.render(scene, spp=ssp)

mi.Bitmap(img).write("cornell_box.path.integrator.spp" + str(ssp) + ".mitsuba.spectral.exr")
