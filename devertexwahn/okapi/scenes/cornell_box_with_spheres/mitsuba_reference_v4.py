import mitsuba as mi

mi.set_variant('scalar_rgb')
#mi.set_variant('llvm_ad_rgb')

scene = mi.load_file("cornell_box_with_spheres.next.mitsuba_v4.xml")

ssp = 1024

img = mi.render(scene, spp=ssp)

mi.Bitmap(img).write("cornell_box_with_spheres.path.integrator.spp" + str(ssp) + ".mitsuba.exr")
