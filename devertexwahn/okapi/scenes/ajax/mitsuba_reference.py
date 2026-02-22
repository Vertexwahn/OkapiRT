import mitsuba as mi
from pathlib import Path

mi.set_variant('scalar_rgb')

# Derive workspace root from this file location: <workspace>/okapi/scenes/ajax/mitsuba_reference.py
workspace_root = Path(__file__).resolve().parents[3]

scene_path = workspace_root / "okapi/scenes/ajax/ajax.constant_emitter.mitsuba.xml"
scene = mi.load_file(str(scene_path))

img = mi.render(scene)

out_path = workspace_root / "okapi/scenes/ajax/ajax.constant_emitter.mitsuba.exr"
mi.Bitmap(img).write(str(out_path))
