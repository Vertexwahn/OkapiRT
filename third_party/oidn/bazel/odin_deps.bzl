## Copyright 2018-2021 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

## DISCLAIMER: Bazel support is community-based. The maintainers do not
## use Bazel internally. The Bazel build can have security risks or
## optimization gaps.

"""External dependencies for odin."""

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def odin_deps():
    """Fetches dependencies of odin."""

    maybe(
        http_archive,
        name = "bazel_skylib",
        sha256 = "b8a1527901774180afc798aeb28c4634bdccf19c4d98e7bdd1ce79d1fe9aaad7",
        urls = [
            "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.4.1/bazel-skylib-1.4.1.tar.gz",
            "https://github.com/bazelbuild/bazel-skylib/releases/download/1.4.1/bazel-skylib-1.4.1.tar.gz",
        ],
    )

    maybe(
        http_archive,
        name = "mkl_dnn_v1",
        build_file = "//bazel/third_party/mkl_dnn:mkldnn_v1.BUILD",
        sha256 = "dc2b9bc851cd8d5a6c4622f7dc215bdb6b32349962875f8bf55cceed45a4c449",
        strip_prefix = "oneDNN-2.7.1",
        urls = [
            "https://github.com/oneapi-src/oneDNN/archive/refs/tags/v2.7.1.tar.gz",
        ],
    )

    maybe(
        http_archive,
        name = "oneTBB",
        urls = [
            "https://github.com/oneapi-src/oneTBB/archive/154cc73ca4d359621202399cc0c3c91058e56e79.zip",
        ],
        strip_prefix = "oneTBB-154cc73ca4d359621202399cc0c3c91058e56e79",
        sha256 = "aa381b093a1149921b23009ce6d4b536c7a29792becb4ea14a47f7bed840b347",
    )
    
    maybe(
        git_repository,
        name = "oidn-weights",
        build_file = "//bazel/third_party:BUILD.oidn-weights",
        commit = "e9cae1b5e7814b8bc2540ff09e77e7ae874b08df",
        #remote = "https://github.com/OpenImageDenoise/oidn-weights",
        remote = "https://github.com/Vertexwahn/oidn-weights-without-git-lfs",
        shallow_since = "1676196087 +0100",
    )
