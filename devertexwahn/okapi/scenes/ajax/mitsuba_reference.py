import mitsuba as mi

mi.set_variant('scalar_rgb')

scene = mi.load_file("okapi/scenes/ajax/ajax.constant_emitter.mitsuba.xml")

img = mi.render(scene)

#mi.Bitmap(img).write("ajax.ao.spp1024.mitsuba.exr")
mi.Bitmap(img).write("/home/vertexwahn/dev/Piper/devertexwahn/ajax.constant_emitter.mitsuba.exr")
