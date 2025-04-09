import mitsuba as mi

mi.set_variant('scalar_rgb')

scene = mi.load_file("ajax.ao.mitsuba.xml")

img = mi.render(scene)

mi.Bitmap(img).write('ajax.ao.spp1024.mitsuba.exr')
