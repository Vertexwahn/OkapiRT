"""
    SPDX-FileCopyrightText: Copyright 2022-2024 Julian Amann <dev@vertexwahn.de>
    SPDX-License-Identifier: Apache-2.0
"""

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def okapi_deps():
    """Fetch external dependencies for Okapi."""

    #-------------------------------------------------------------------------------------
    # rules_vulkan
    #-------------------------------------------------------------------------------------

    maybe(
        native.local_repository,
        name = "rules_7zip",
        path = "../third_party/rules_7zip",
    )

    maybe(
        native.local_repository,
        name = "com_github_zaucy_rules_vulkan",
        path = "../third_party/rules_vulkan",
    )

    #-------------------------------------------------------------------------------------
    # emsdk
    #-------------------------------------------------------------------------------------

    # Use local repository
    maybe(
        native.local_repository,
        name = "emsdk",
        path = "../third_party/emsdk/bazel",
    )

    # Use http archive from github.com
    #maybe(
    #    http_archive,
    #    name = "emsdk",
    #    sha256 = "a41dccfd15be9e85f923efaa0ac21943cbab77ec8d39e52f25eca1ec61a9ac9e",
    #    strip_prefix = "emsdk-3.0.0/bazel",
    #    url = "https://github.com/emscripten-core/emsdk/archive/refs/tags/3.0.0.tar.gz",
    #)
