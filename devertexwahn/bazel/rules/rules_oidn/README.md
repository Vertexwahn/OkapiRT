[![Support Ukraine](https://img.shields.io/badge/Support-Ukraine-FFD500?style=flat&labelColor=005BBB)](https://opensource.fb.com/support-ukraine)

# rules_oidn -- Bazel build rules for Intel Open Image Denoise

Rules for using [Intel Open Image Denoise](https://www.openimagedenoise.org/) in your Bazel builds.

## What it does?

Open Image Denoise can be used to denoise your images.
Given a noisy input image Open Image Denoise can produce a denoised output image:

![Denoised](docs/oidn.png)

More comparison images can be found [here](docs/denoised.pdf).

`rules_oidn` helps you to embed Open Image Denoise 1.4.3 into your Bazel builds.

## Current status

Currently, these rules compile on Ubuntu 22.04 (other Linux distributions should also work), Windows and macOS (Intel and arm64).

The goal of `rules_oidn` is not to reproduce a 1:1 binary compatible library that is equal to a CMake build.
The focus is on the compilation of a working library that can be used in Bazel projects and is close to the CMake build.
The following table compares the precompiled version of OIDN to this Bazel build:

| Dependency                            | Bazel                                    | [Precompiled Open Image Denoise v1.4.3](https://github.com/OpenImageDenoise/oidn/releases/tag/v1.4.3) |
|---------------------------------------|------------------------------------------|-------------------------------------------------------------------------------------------------------|
| oneDNN                                | 2.7.3                                    | 2.2.4                                                                                                 |
| Intel® Implicit SPMD Program Compiler | v1.19.0                                  | ?                                                                                                     |
| ComputeLibrary                        | 22.11                                    | ?                                                                                                     |
| oneTBB                                | 5b2a49008c32c7ba3df7c9e137a894ff53710b64 | oneTBB 2021.5.0                                                                                       |

## Quick start

**Prerequisites:**

The following tools should be installed:

- [Git](https://git-scm.com/)
- [Bazel](https://bazel.build/install)
- A C++ compiler (GCC, Visual Studio, Clang, etc.)

**Checkout, build, and run:**

*All platforms:*

```shell
git clone https://github.com/Vertexwahn/rules_oidn.git
cd rules_oidn
cd tests
```

*Run example with Ubuntu 22.04:*

```shell
bazel run --config=gcc11 //:example
```

*Run example with Visual Studio 2022:*

```shell
bazel run --config=vs2022 //:example
```

*Run example with macOS:*

```shell
bazel run --config=macos //:example
```

See [tests/.bazelrc](tests/.bazelrc) for other supported build configs.

You can also provide the filenames of the input and output image as an argument, e.g. on Ubuntu 22.04:

```shell
bazel run --config=gcc11 //:example -- --input=${HOME}/rules_oidn/tests/data/cornell_box.naive_diffuse.box_filter.spp64.embree.exr --output=${HOME}/denoised_spp64.exr
```

## More about the example

The example provides a noisy rendering of the Cornell Box.

![Noisy](tests/data/noisy_10spp.png)

Additionally, there is a corresponding albedo image:

![Noisy](tests/data/albedo_10spp.png)

And a normal image:

![Noisy](tests/data/normal_10spp.png)

Currently, only the noisy rendering serves as an input for the example. 
The albedo and normal images are not used.
But the example code can be easily modified to use them.

## Generating weights manually

```shell
python3 .\scripts\blob_to_cpp.py .\weights\rt_alb.tza -o .\weights\rt_alb.tza.cpp -H .\weights\rt_alb.tza.h
python3 .\scripts\blob_to_cpp.py .\weights\rt_hdr.tza -o .\weights\rt_hdr.tza.cpp -H .\weights\rt_hdr.tza.h
python3 .\scripts\blob_to_cpp.py .\weights\rt_hdr_alb.tza -o .\weights\rt_hdr_alb.tza.cpp -H .\weights\rt_hdr_alb.tza.h
python3 .\scripts\blob_to_cpp.py .\weights\rt_hdr_alb_nrm.tza -o .\weights\rt_hdr_alb_nrm.tza.cpp -H .\weights\rt_hdr_alb_nrm.tza.h
python3 .\scripts\blob_to_cpp.py .\weights\rt_hdr_calb_cnrm.tza -o .\weightsrt_hdr_calb_cnrm.tza.cpp -H .\weights\rt_hdr_calb_cnrm.tza.h
python3 .\scripts\blob_to_cpp.py .\weights\rt_ldr.tza -o .\weights\rt_ldr.tza.cpp -H .\weights\rt_ldr.tza.h
python3 .\scripts\blob_to_cpp.py .\weights\rt_ldr_alb.tza -o .\weights\rt_ldr_alb.tza.cpp -H .\weights\rt_ldr_alb.tza.h
python3 .\scripts\blob_to_cpp.py .\weights\rt_ldr_alb_nrm.tza -o .\weights\rt_ldr_alb_nrm.tza.cpp -H .\weights\rt_ldr_alb_nrm.tza.h
python3 .\scripts\blob_to_cpp.py .\weights\rt_ldr_calb_cnrm.tza -o .\weights\rt_ldr_calb_cnrm.tza.cpp -H .\weights\rt_ldr_calb_cnrm.tza.h
python3 .\scripts\blob_to_cpp.py .\weights\rt_nrm.tza -o .\weights\rt_nrm.tza.cpp -H .\weights\rt_nrm.tza.h
python3 .\scripts\blob_to_cpp.py .\weights\rtlightmap_dir.tza -o .\weights\rtlightmap_dir.tza.cpp -H .\weights\rtlightmap_dir.tza.h
python3 .\scripts\blob_to_cpp.py .\weights\rtlightmap_hdr.tza -o .\weights\rtlightmap_hdr.tza.cpp -H .\weights\rtlightmap_hdr.tza.h
```

## Reading material

- [DNND 1: a Deep Neural Network Dive](https://maxliani.wordpress.com/2023/03/17/dnnd-1-a-deep-neural-network-dive/)
- [Declan Russell's blog: Intels Open Image AI denoiser](https://declanrussell.com/portfolio/intel-open-image-denoiser-2/)
- [Bazelizing Open Image Denoise — Part 1: The journey begins](https://medium.com/@Vertexwahn/bazelizing-open-image-denoise-part-1-the-journey-begins-a16c78ea1b88)
- [Bazelizing Open Image Denoise — Part 2: More details about the bazelization](https://medium.com/@Vertexwahn/bazelizing-open-image-denoise-part-2-more-details-about-the-bazelization-aff1bf904798)
- [Bazelizing Open Image Denoise — Part 3: A failing unit test](https://medium.com/@Vertexwahn/bazelizing-open-image-denoise-part-3-a-failing-unit-test-695fb829931c)
- [Bazelizing Open Image Denoise — Part 4: Issue with black stipes solved](https://medium.com/@Vertexwahn/bazelizing-open-image-denoise-part-4-issue-with-black-stipes-solved-b37b1d3a3b52)

## License

This work is published under the Apache 2.0 License.

### Notes on copyright

This repository contains code copied from [TensorFlow](https://github.com/tensorflow/tensorflow) which is also under the Apache 2.0 License.
The copyright of the corresponding files belongs to the TensorFlow authors.
