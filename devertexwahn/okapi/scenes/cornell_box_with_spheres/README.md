# Cornell Box with Spheres

![Conell Box with Spheres](cornell_box_with_spheres.next.png)

```shell
bazel run --config=gcc13 --compilation_mode=opt //okapi/ui:okapi.ui -- \
--scene_filename=${HOME}/dev/Piper/devertexwahn/okapi/scenes/cornell_box_with_spheres/cornell_box_with_spheres.next.okapi.xml \
--samples_per_pixel=8192 \
--film_filename=cornell_box_with_spheres.next.exr
```

## Variant 2:

![Conell Box with Spheres](cornell_box_with_spheres_v2.next.png)

```shell
bazel run --config=gcc13 --compilation_mode=opt //okapi/ui:okapi.ui -- \
--scene_filename=${HOME}/dev/Piper/devertexwahn/okapi/scenes/cornell_box_with_spheres/cornell_box_with_spheres.next.okapi_v2.xml \
--samples_per_pixel=8192 \
--film_filename=cornell_box_with_spheres_v2.next.exr
```

## Variant 3:

![Conell Box with Spheres](cornell_box_with_spheres_v3.next.png)

```shell
bazel run --config=gcc13 --compilation_mode=opt //okapi/ui:okapi.ui -- \
--scene_filename=${HOME}/dev/Piper/devertexwahn/okapi/scenes/cornell_box_with_spheres/cornell_box_with_spheres.next.okapi_v3.xml \
--samples_per_pixel=8192 \
--film_filename=cornell_box_with_spheres_v3.next.exr \
--thread_count=6
```

## Variant 4 (mirror + smooth dielectric material):

![Conell Box with Spheres](cornell_box_with_dielectric_sphere.exr.spp40000.webp)

*Ubuntu 24.04*

```shell
bazel run --config=gcc13 --compilation_mode=opt //okapi/ui:okapi.ui -- \
--scene_filename=${HOME}/dev/Piper/devertexwahn/okapi/scenes/cornell_box_with_spheres/cornell_box_with_dielectric_sphere.xml \
--samples_per_pixel=8192 \
--film_filename=cornell_box_with_dielectric_sphere.exr
```

*Windows*

```shell
bazel --output_base=C:/bazel_output_base run --config=vs2022 --compilation_mode=opt //okapi/ui:okapi.ui -- --scene_filename=C:/dev/Piper/devertexwahn/okapi/scenes/cornell_box_with_spheres/cornell_box_with_dielectric_sphere.xml --samples_per_pixel=8192 --film_filename=cornell_box_with_dielectric_sphere.exr
```

Reference rendering with Mitsuba:

```shell
python3 mitsuba_reference_v4.py
```

![Conell Box with Spheres](cornell_box_with_spheres.path.integrator.spp1024.mitsuba.png)
