# Ajax

## Ubuntu 20.04

```shell
bazel run --config=gcc9 --compilation_mode=opt //okapi/cli:okapi.cli -- /home/vertexwahn/dev/Piper/devertexwahn/okapi/scenes/ajax/ajax.normal.spp1.octree.xml
```

## Ubuntu 22.04

```shell
bazel run --config=gcc11 --compilation_mode=opt //okapi/ui:okapi.ui -- \
--scene_filename=${HOME}/dev/Piper/devertexwahn/okapi/scenes/ajax/ajax.ao.okapi.xml \
--samples_per_pixel=1024
```

## Simple Integrator

## Ubuntu 22.04

```shell
bazel run --config=gcc11 --compilation_mode=opt //okapi/ui:okapi.ui -- \
--scene_filename=${HOME}/dev/Piper/devertexwahn/okapi/scenes/ajax/ajax.simple.okapi.xml \
--samples_per_pixel=100 \
--intersector=octree
```
