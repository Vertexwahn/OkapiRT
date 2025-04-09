# Readme

```shell
bazel run --config=gcc13 --compilation_mode=opt //okapi/ui:okapi.ui -- \
--scene_filename=${HOME}/dev/Piper/devertexwahn/okapi/scenes/sphere/sphere.rtiow.okapi.xml \
--samples_per_pixel=10000
```
