<!--
SPDX-FileCopyrightText: Copyright 2022-2025 Julian Amann <dev@vertexwahn.de>
SPDX-License-Identifier: Apache-2.0
-->

[![Support Ukraine](https://img.shields.io/badge/Support-Ukraine-FFD500?style=flat&labelColor=005BBB)](https://opensource.fb.com/support-ukraine)
[![Build Status](https://vertexwahn.visualstudio.com/FlatlandRT/_apis/build/status/Vertexwahn.FlatlandRT?branchName=master)](https://vertexwahn.visualstudio.com/FlatlandRT/_build/latest?definitionId=6&branchName=master)

# Okapi Renderer

## Description

Okapi is a 3D ray tracer. 
The following image has been rendered with Okapi
(Modeled by Mac Ptaszynski. Model available [here](https://www.behance.net/gallery/20046385/BMW-315-DA2))

![BMW 315 DA2](screenshots/BMW_315_DA2.ao_9f93bf4.png)

## Quickstart

This project uses [Bazel](https://bazel.build/) as a build system. The current used version is defined in [.bazelversion](devertexwahn/.bazelversion).

**Prerequisites:**

The following tools should be installed:

- [Git](https://git-scm.com/)
- [Bazel](https://bazel.build/install)
- A C++ compiler ([GCC](https://gcc.gnu.org/), [Visual Studio](https://visualstudio.microsoft.com/), [Clang](https://clang.llvm.org/), etc.)

**Checkout, build, and run:**

*All platforms:*

```shell
git clone https://github.com/Vertexwahn/Piper.git # clone the repository
cd Piper # change directory to cloned repository
cd devertexwahn # switch to the location where the WORKSPACE.bazel file is located
```

### Command line interface

You can use the Okapi Renderer command line interface by invoking one of the following commands:

*Render a scene with Windows 10/11 x64 with Visual Studio 2019:*

```shell
bazel run --config=vs2019 --compilation_mode=opt //okapi/cli:okapi.cli -- --scene_filename=C:\scenes\okapi\scenes\ajax\ajax.ao.okapi.xml
```

*Render a scene with Windows 10/11 x64 with Visual Studio 2022:*

```shell
bazel run --config=vs2022 --compilation_mode=opt //okapi/cli:okapi.cli -- --scene_filename=C:\scenes\okapi\scenes\ajax\ajax.ao.okapi.xml
```

*Render a scene with Ubuntu 22.04:*

```shell
bazel run --config=gcc11 --compilation_mode=opt //okapi/cli:okapi.cli -- --scene_filename=$(pwd)/okapi/scenes/ajax/ajax.ao.okapi.xml
```

*Render a scene with Ubuntu 24.04:*

```shell
bazel run --config=gcc13 --compilation_mode=opt //okapi/cli:okapi.cli -- --scene_filename=$(pwd)/okapi/scenes/ajax/ajax.ao.okapi.xml
```

*Render a scene with macOS 11/12:*

```shell
bazel run --config=macos --compilation_mode=opt //okapi/cli:okapi.cli -- --scene_filename=${HOME}/dev/Piper/devertexwahn/okapi/scenes/ajax/ajax.ao.okapi.xml
```

### User Interface

*Render a scene with Windows 10/11 x64 with Visual Studio 2019:*

```shell
bazel --output_base=G:/bazel_output_base run --config=vs2022 --compilation_mode=opt //okapi/ui:okapi.ui
```

*Render a scene with Windows 10/11 x64 with Visual Studio 2022:*

```shell
bazel --output_base=G:/bazel_output_base run --config=vs2022 --compilation_mode=opt //okapi/ui:okapi.ui
```

*Render a scene with Ubuntu 22.04:*

```shell
bazel run --config=gcc11 --compilation_mode=opt //okapi/ui:okapi.ui
```

![UI Ubuntu 20.04](screenshots/Screenshot%20from%202022-01-23%2015-02-35.png)

*Render a scene with macOS 11/12:*

```shell
bazel run --config=macos --compilation_mode=opt //okapi/ui:okapi.ui
```

![UI Ubuntu 20.04](screenshots/macos%202022-02-16.png)

## Building

### Ubuntu 22.04

#### Command line (bash/zsh)

```shell
# Run all tests using GCC 11.2
bazel test --config=gcc11 //...
# Build all targets using GCC 11.2
bazel build --config=gcc11 //... 
# Run all tests using Clang14 (ree can not be build using Clang14)
bazel test --config=clang14 -- //... 
# Build all targets using Clang14 (ree can not be build using Clang14)
bazel build --config=clang14 -- //... 
```

#### CLion

There is a Bazel Plugin for [CLion](https://www.jetbrains.com/clion/). It can be downloaded from [here](https://plugins.jetbrains.com/author/4bb31785-ad06-4671-8e26-266aadc184bd).

You can use the following `.bazelproject` file:

```yaml
directories:
  .

test_sources:
  flatland/tests
  okapi/tests

derive_targets_from_directories: true

additional_languages:
  python

build_flags:
  --config=gcc13 # on Ubuntu 24.04
  #--config=macos # on macOS 15
  #--config=vs2022 # on Windows 11
  #--config=clang19
```

#### Clang 14

Bazel supports different toolchains. 
Usually gcc is used a C++ default compiler when using Ubuntu 22.04.
But you can also easily use Clang 14 to compile Okapi.

```shell
# Build with LLVM
bazel build --config=clang14 //...
# Test with LLVM
bazel test --config=clang14 //...
```

#### Remote Build Cache

When switching configuration (`--config=gcc11` vs. `--config=clang14`) it turns out that using a (local) remote build cache server results in better build performance.

You can spin up your own local instance via:

```shell
sudo docker run -u 1000:1000 -v ~/bazel_remote_cache:/data \                       
        -p 9090:8080 -p 9092:9092 buchgr/bazel-remote-cache --max_size=30
```

You can set up a home RC file (`.bazelrc`) to use the remote build cache:

```shell
cd ~
echo "build --remote_cache=http://localhost:9090" >> .bazelrc
```

#### Code coverage

Make sure that lcov is installed.

    sudo apt install lcov

Go to the directory that contains the `WORKSPACE.bazel` file and execute:

```shell
./coverage.sh --additional_bazel_config=buchgr_remote_cache
```

#### Address Sanitizer

There is a build config called `asan` that can be used for detection of memory errors.

    bazel run --config=asan --compilation_mode=opt //okapi/cli:okapi.cli ~/scenes/sphere.okapi.xml

#### Clang Tidy

    bazel build //flatland.cli:flatland --aspects clang_tidy/clang_tidy.bzl%clang_tidy_aspect --output_groups=report

### Windows 10/11

#### Command line (Powershell)

    # Build with Visual Studio 2022 C++ Compiler
    bazel build --config=vs2022 //...

#### Using Visual Studio 2022

Use [Lavender](https://github.com/tmandry/lavender) to generate a solution and project files for Visual Studio.

```shell
python3 D:\dev\lavender\generate.py --config=vs2022  //...
```

Lavender is far from being perfect. 
It might be necessary to do some modifications to the generated solution and project files.

### macOS

```shell
bazel build --config=clang14 //...
```

## Development process

I made a short video where I describe how I use test driven development to implement this project: 

[![Let's Code: Using Test-driven Development to implement a ray tracer](https://img.youtube.com/vi/vFBXNr952nU/0.jpg)](https://www.youtube.com/watch?v=vFBXNr952nU)

## License

See [LICENSE.md](../LICENSE.md).

## Copyright notes

The Okapi Project makes use of several software libraries. 
Besides this, 
some source code was directly copied from other open-source software libraries or programs. 
This is always clearly stated as a comment in the source code of Flatland. 
Additionally, some tools where copied to this repository.
The corresponding licenses can be found in the Licenses folder distributed with this source code:

### Copied source code/ideas

* Mitsuba Renderer 2 (https://github.com/mitsuba-renderer/mitsuba2) (scene file format) ([License](licenses/mitsuba2/LICENSE))
* pbrt, Version 3 (https://github.com/mmp/pbrt-v3) (Refract, face_forward functions) ([License](licenses/pbrt-v3/LICENSE.txt))
* pbrt, Version 4 (https://github.com/mmp/pbrt-v4) (concentric sampling of unit disk) ([License](licenses/pbrt-v4/LICENSE.txt))
* bazel_clang_tidy (https://github.com/erenon/bazel_clang_tidy) (almost everything) ([License](licenses/bazel_clang_tidy/LICENSE))
* appleseed (https://github.com/appleseedhq/appleseed)
* 
### Build related

* LLVM toolchain for Bazel (https://github.com/grailbio/bazel-toolchain) (building Flatland with LLVM) ([License](licenses/llvm_bazel_toolchain/LICENSE))

### Third-party dependencies

* {fmt} (https://github.com/fmtlib/fmt) (third party dependency) ([License](licenses/fmt/LICENSE.rst))
* Boost (https://www.boost.org/) (third party dependency) ([License](licenses/boost/LICENSE))
* catch2 (https://github.com/catchorg/Catch2) (third party dependency) ([License](licenses/catch2/LICENSE.txt))
* gflags (https://github.com/gflags/gflags/) (third party dependency) ([License](licenses/gflags/COPYING.txt))
* glog (https://github.com/google/glog) (third party dependency) ([License](licenses/glog/COPYING))
* Google Test (https://github.com/google/googletest) (third party dependency) ([License](licenses/googletest/LICENSE))
* hypothesis (https://github.com/wjakob/hypothesis) (third party dependency) ([License](licenses/hypothesis/LICENSE))
* pcg-cpp (https://github.com/imneme/pcg-cpp/)  (third party dependency) ([License](licenses/pcg-cpp/LICENSE-MIT.txt))
* pugixml (https://pugixml.org/, https://github.com/zeux/pugixml) (third party dependency) ([License](licenses/pugixml/LICENSE.md))
* yaml-cpp (https://github.com/jbeder/yaml-cpp) (third party dependency) ([License](../../yaml-cpp-c73ee34704c512ebe915b283645aefa9f424a22f/LICENSE))
* zlib (https://zlib.net/) ([License](../../third_party/zlib-1.2.12/README))

### Tools

* Bazelisk (https://github.com/bazelbuild/bazelisk) ([License](licenses/bazelisk/)) 

### Artwork

The Stanford Bunny was derived from the Stanford Bunny provided from the Stanford 3D Scanning Repository (see [here](http://graphics.stanford.edu/data/3Dscanrep/#bunny)).

### Credits

A big thank goes to all the providers, developers and maintainers of the aforementioned artifacts.
