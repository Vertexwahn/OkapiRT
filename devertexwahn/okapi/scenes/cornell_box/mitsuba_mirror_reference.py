import mitsuba as mi

mi.set_variant('scalar_rgb')
#mi.set_variant('llvm_ad_rgb')

scene = mi.load_file("cornell_box.mirror.path.integrator.spp1.mitsuba.xml")

ssp = 1024

img = mi.render(scene, spp=ssp)

mi.Bitmap(img).write("cornell_box.mirror.path.integrator.spp" + str(ssp) + ".mitsuba.exr")
