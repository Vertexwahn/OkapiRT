# Cube

*Ubuntu 24.04*

```shell
bazel run --config=gcc13 --compilation_mode=opt //okapi/ui:okapi.ui -- \
--scene_filename=${HOME}/dev/Piper/devertexwahn/okapi/scenes/cube/cube.normal.okapi.xml \
--samples_per_pixel=100
```
