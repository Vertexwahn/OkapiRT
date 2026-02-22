import mitsuba as mi
import matplotlib.pyplot as plt
from pathlib import Path

mi.set_variant('scalar_rgb')

# Derive workspace root from this file location: <workspace>/okapi/scenes/ajax/mitsuba_reference.py
workspace_root = Path(__file__).resolve().parents[3]

scene_path = workspace_root / "okapi/scenes/cornell_box_with_spheres/cornell_box_with_spheres.next.mitsuba_v4.xml"
scene = mi.load_file(str(scene_path))

ssp = 100

image = mi.render(scene, spp=ssp)

plt.axis("off")
plt.imshow(image ** (1.0 / 2.2)); # approximate sRGB tonemapping
plt.show()

params = mi.traverse(scene)
print(params)

out_path = workspace_root / ("okapi/scenes/cornell_box_with_spheres/cornell_box_with_spheres.path.integrator.spp" + str(ssp) + ".mitsuba.exr")
mi.Bitmap(image).write(str(out_path))
