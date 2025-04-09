## Modified Phong BRDF

# Render Scene

```shell
bazel run --config=gcc11 --compilation_mode=opt //okapi/ui:okapi.ui -- \
--scene_filename=${HOME}/dev/Piper/devertexwahn/okapi/scenes/cornell_box_with_spheres/modified_phong_brdf.okapi.xml \
--samples_per_pixel=16 \
--integrator=next \
--film_filename=cornell_box.modified_phong_brdf.spp1.next.exr
```
