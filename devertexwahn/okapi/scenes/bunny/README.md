# Stanford Bunny

http://graphics.stanford.edu/data/3Dscanrep/

![Standford Bunny rendered with ambient occlusion](bunny.ao.png)

#### UI

*macOS*

```shell
bazel run --config=macos --compilation_mode=opt //okapi/ui:okapi.ui -- \
--scene_filename=${HOME}/dev/Piper/devertexwahn/okapi/scenes/bunny/bunny.normal.okapi.xml \
--samples_per_pixel=100
```

```shell
bazel run --config=macos --compilation_mode=opt //okapi/ui:okapi.ui -- \
--scene_filename=${HOME}/dev/Piper/devertexwahn/okapi/scenes/bunny/bunny.ao.okapi.xml \
--samples_per_pixel=100
```

```shell
bazel run --config=macos --compilation_mode=opt //okapi/ui:okapi.ui -- \
--scene_filename=${HOME}/dev/Piper/devertexwahn/okapi/scenes/bunny/bunny.hit.okapi.xml \
--samples_per_pixel=100
```

```shell
bazel run --config=macos --compilation_mode=opt //okapi/ui:okapi.ui -- \
--scene_filename=/Users/vertexwahn/dev/Piper/devertexwahn/okapi/scenes/bunny/bunny.hit.okapi.xml \
--samples_per_pixel=100
```

*Ubuntu 24.04*

```shell
bazel run --config=gcc13 --compilation_mode=opt //okapi/ui:okapi.ui -- \
--scene_filename=${HOME}/dev/Piper/devertexwahn/okapi/scenes/bunny/matpreview.okapi.xml \
--samples_per_pixel=100
```

*Windows with Visual Studio 2022*

```shell
bazel --output_base=C:/bazel_output_base run --config=vs2022 --compilation_mode=opt //okapi/ui:okapi.ui -- --scene_filename=C:/dev/Piper/devertexwahn/okapi/scenes/bunny/bunny.normal.okapi.xml --samples_per_pixel=100 --film_filename=scene.exr
```
