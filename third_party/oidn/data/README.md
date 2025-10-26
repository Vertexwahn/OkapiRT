# Test data

## Cornel box scene

### Box filter

```shell
spp_list=(1 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536)
#name=(1 2 4 8 16 32 64 128 256 512 1024 2048 4k 4k 16k 32k 65k)

for spp in "${spp_list[@]}"
do
  echo "$spp"

  bazel run --config=gcc11 --compilation_mode=opt //okapi/cli:okapi.cli -- \
    --scene_filename=${HOME}/dev/Piper/devertexwahn/rules_oidn/tests/data/cornell_box.naive.diffuse.box_filter.okapi.xml \
    --samples_per_pixel="$spp" \
    --film_filename=cornell_box.naive_diffuse.box_filter.spp"$spp".embree.exr
done
```

### Gaussian filter

```shell
spp_list=(1 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536)
#name=(1 2 4 8 16 32 64 128 256 512 1024 2048 4k 4k 16k 32k 65k)

for spp in "${spp_list[@]}"
do
  echo "$spp"

  bazel run --config=gcc11 --compilation_mode=opt //okapi/cli:okapi.cli -- \
    --scene_filename=${HOME}/dev/Piper/devertexwahn/rules_oidn/tests/data/cornell_box.naive.diffuse.gaussian_filter.okapi.xml \
    --samples_per_pixel="$spp" \
    --film_filename=cornell_box.naive_diffuse.gaussian_filter.spp"$spp".embree.exr
done
```

### Denoised images

```shell
SET FILE_EXTENSION=exr
SET PATH_TO_DENOISER=C:\Denoiser_v1.5
SET OUTPUT_PREFIX=denoised_

for /r %%v in (*.%FILE_EXTENSION%) do %PATH_TO_DENOISER%\Denoiser.exe -i "%%~nv.%FILE_EXTENSION%" -o "%OUTPUT_PREFIX%%%~nv.%FILE_EXTENSION%"

cmd /k
```
