# Copied from TensorFlow Apache-2.0 license
# https://github.com/tensorflow/tensorflow/blob/master/third_party/mkl_dnn/mkldnn_v1.BUILD

exports_files(["LICENSE"])

#load("@org_tensorflow//tensorflow:tensorflow.bzl", "tf_openmp_copts")
#load("@org_tensorflow//third_party/mkl:build_defs.bzl", "if_mkl")
#load("@org_tensorflow//third_party/mkl_dnn:build_defs.bzl", "if_mkldnn_openmp")
#load("@org_tensorflow//third_party/mkl:build_defs.bzl", "if_mkl_ml")
load("@bazel_skylib//rules:expand_template.bzl", "expand_template")

_CMAKE_COMMON_LIST = {
    "#cmakedefine DNNL_GPU_RUNTIME DNNL_RUNTIME_${DNNL_GPU_RUNTIME}": "#define DNNL_GPU_RUNTIME DNNL_RUNTIME_NONE",
    "#cmakedefine DNNL_USE_RT_OBJECTS_IN_PRIMITIVE_CACHE": "#undef DNNL_USE_RT_OBJECTS_IN_PRIMITIVE_CACHE",
    "#cmakedefine DNNL_WITH_SYCL": "#undef DNNL_WITH_SYCL",
    "#cmakedefine DNNL_WITH_LEVEL_ZERO": "#undef DNNL_WITH_LEVEL_ZERO",
    "#cmakedefine DNNL_SYCL_CUDA": "#undef DNNL_SYCL_CUDA",
    "#cmakedefine DNNL_SYCL_HIP": "#undef DNNL_SYCL_HIP",
    "#cmakedefine DNNL_ENABLE_STACK_CHECKER": "#undef DNNL_ENABLE_STACK_CHECKER",
    "#cmakedefine DNNL_EXPERIMENTAL": "#undef DNNL_EXPERIMENTAL",
    "#cmakedefine01 BUILD_TRAINING": "#define BUILD_TRAINING 1",
    "#cmakedefine01 BUILD_INFERENCE": "#define BUILD_INFERENCE 0",
    "#cmakedefine01 BUILD_PRIMITIVE_ALL": "#define BUILD_PRIMITIVE_ALL 1",
    "#cmakedefine01 BUILD_BATCH_NORMALIZATION": "#define BUILD_BATCH_NORMALIZATION 0",
    "#cmakedefine01 BUILD_BINARY": "#define BUILD_BINARY 0",
    "#cmakedefine01 BUILD_CONCAT": "#define BUILD_CONCAT 0",
    "#cmakedefine01 BUILD_CONVOLUTION": "#define BUILD_CONVOLUTION 0",
    "#cmakedefine01 BUILD_DECONVOLUTION": "#define BUILD_DECONVOLUTION 0",
    "#cmakedefine01 BUILD_ELTWISE": "#define BUILD_ELTWISE 0",
    "#cmakedefine01 BUILD_INNER_PRODUCT": "#define BUILD_INNER_PRODUCT 0",
    "#cmakedefine01 BUILD_LAYER_NORMALIZATION": "#define BUILD_LAYER_NORMALIZATION 0",
    "#cmakedefine01 BUILD_LRN": "#define BUILD_LRN 0",
    "#cmakedefine01 BUILD_MATMUL": "#define BUILD_MATMUL 0",
    "#cmakedefine01 BUILD_POOLING": "#define BUILD_POOLING 0",
    "#cmakedefine01 BUILD_PRELU": "#define BUILD_PRELU 0",
    "#cmakedefine01 BUILD_REDUCTION": "#define BUILD_REDUCTION 0",
    "#cmakedefine01 BUILD_REORDER": "#define BUILD_REORDER 0",
    "#cmakedefine01 BUILD_RESAMPLING": "#define BUILD_RESAMPLING 0",
    "#cmakedefine01 BUILD_RNN": "#define BUILD_RNN 0",
    "#cmakedefine01 BUILD_SHUFFLE": "#define BUILD_SHUFFLE 0",
    "#cmakedefine01 BUILD_SOFTMAX": "#define BUILD_SOFTMAX 0",
    "#cmakedefine01 BUILD_SUM": "#define BUILD_SUM 0",
    "#cmakedefine01 BUILD_PRIMITIVE_CPU_ISA_ALL": "#define BUILD_PRIMITIVE_CPU_ISA_ALL 1",
    "#cmakedefine01 BUILD_SSE41": "#define BUILD_SSE41 0",
    "#cmakedefine01 BUILD_AVX2": "#define BUILD_AVX2 0",
    "#cmakedefine01 BUILD_AVX512": "#define BUILD_AVX512 0",
    "#cmakedefine01 BUILD_AMX": "#define BUILD_AMX 0",
    "#cmakedefine01 BUILD_PRIMITIVE_GPU_ISA_ALL": "#define BUILD_PRIMITIVE_GPU_ISA_ALL 0",
    "#cmakedefine01 BUILD_GEN9": "#define BUILD_GEN9 0",
    "#cmakedefine01 BUILD_GEN11": "#define BUILD_GEN11 0",
    "#cmakedefine01 BUILD_XELP": "#define BUILD_XELP 0",
    "#cmakedefine01 BUILD_XEHPG": "#define BUILD_XEHPG 0",
    "#cmakedefine01 BUILD_XEHPC": "#define BUILD_XEHPC 0",
    "#cmakedefine01 BUILD_XEHP": "#define BUILD_XEHP 0",
}

_DNNL_RUNTIME_OMP = {
    "#cmakedefine DNNL_CPU_THREADING_RUNTIME DNNL_RUNTIME_${DNNL_CPU_THREADING_RUNTIME}": "#define DNNL_CPU_THREADING_RUNTIME DNNL_RUNTIME_OMP",
    "#cmakedefine DNNL_CPU_RUNTIME DNNL_RUNTIME_${DNNL_CPU_RUNTIME}": "#define DNNL_CPU_RUNTIME DNNL_RUNTIME_OMP",
}

_DNNL_RUNTIME_OMP.update(_CMAKE_COMMON_LIST)

_DNNL_RUNTIME_THREADPOOL = {
    "#cmakedefine DNNL_CPU_THREADING_RUNTIME DNNL_RUNTIME_${DNNL_CPU_THREADING_RUNTIME}": "#define DNNL_CPU_THREADING_RUNTIME DNNL_RUNTIME_THREADPOOL",
    "#cmakedefine DNNL_CPU_RUNTIME DNNL_RUNTIME_${DNNL_CPU_RUNTIME}": "#define DNNL_CPU_RUNTIME DNNL_RUNTIME_THREADPOOL",
}

_DNNL_RUNTIME_THREADPOOL.update(_CMAKE_COMMON_LIST)

expand_template(
    name = "dnnl_config_h",
    out = "include/oneapi/dnnl/dnnl_config.h",
    substitutions = select({
        #"@org_tensorflow//third_party/mkl_dnn:build_with_mkldnn_openmp": _DNNL_RUNTIME_OMP,
        "//conditions:default": _DNNL_RUNTIME_THREADPOOL,
    }),
    template = "include/oneapi/dnnl/dnnl_config.h.in",
)

# Create the file dnnl_version.h with DNNL version numbers.
# Currently, the version numbers are hard coded here. If DNNL is upgraded then
# the version numbers have to be updated manually. The version numbers can be
# obtained from the PROJECT_VERSION settings in CMakeLists.txt. The variable is
# set to "version_major.version_minor.version_patch". The git hash version can
# be set to NA.
# TODO(agramesh1): Automatically get the version numbers from CMakeLists.txt.
expand_template(
    name = "dnnl_version_h",
    out = "include/oneapi/dnnl/dnnl_version.h",
    substitutions = {
        "@DNNL_VERSION_MAJOR@": "2",
        "@DNNL_VERSION_MINOR@": "7",
        "@DNNL_VERSION_PATCH@": "3",
        "@DNNL_VERSION_HASH@": "N/A",
    },
    template = "include/oneapi/dnnl/dnnl_version.h.in",
)

_COPTS_LIST = select({
    #"@org_tensorflow//tensorflow:windows": [],
    "@platforms//os:windows": [],
    "//conditions:default": ["-fexceptions"],
}) + [
    "-UUSE_MKL",
    "-UUSE_CBLAS",
    "-DDNNL_ENABLE_MAX_CPU_ISA",
]  # + tf_openmp_copts()

_INCLUDES_LIST = [
    "include",
    "src",
    "src/common",
    "src/common/ittnotify",
    "src/cpu",
    "src/cpu/gemm",
    "src/cpu/x64/xbyak",
]

_TEXTUAL_HDRS_LIST = glob([
    "include/**/*",
    "src/common/*.hpp",
    "src/common/ittnotify/**/*.h",
    "src/cpu/*.hpp",
    "src/cpu/**/*.hpp",
    "src/cpu/jit_utils/**/*.hpp",
    "src/cpu/x64/xbyak/*.h",
]) + [
    ":dnnl_config_h",
    ":dnnl_version_h",
]

# Large autogen files take too long time to compile with usual optimization
# flags. These files just generate binary kernels and are not the hot spots,
# so we factor them out to lower compiler optimizations in ":dnnl_autogen".
# Using -O1 to enable optimizations to reduce stack consumption. (With -O0,
# compiler doesn't clean up stack from temporary objects.)
cc_library(
    name = "onednn_autogen",
    srcs = glob(["src/cpu/x64/gemm/**/*_kern_autogen*.cpp"]),
    copts = [
        "-O1",
        "-U_FORTIFY_SOURCE",
    ] + _COPTS_LIST,
    includes = _INCLUDES_LIST,
    textual_hdrs = _TEXTUAL_HDRS_LIST,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "onednn",
    srcs = glob(
        [
            "src/common/*.cpp",
            "src/cpu/*.cpp",
            "src/cpu/**/*.cpp",
            "src/common/ittnotify/*.c",
            "src/cpu/jit_utils/**/*.cpp",
        ],
        exclude = [
            "src/cpu/aarch64/**",
            "src/cpu/x64/gemm/**/*_kern_autogen.cpp",
        ],
    ),
    copts = _COPTS_LIST,
    includes = _INCLUDES_LIST,
    # TODO(penpornk): Use lrt_if_needed from tensorflow.bzl instead.
    linkopts = select({
        #"@org_tensorflow//tensorflow:linux_aarch64": ["-lrt"],
        #"@org_tensorflow//tensorflow:linux_x86_64": ["-lrt"],
        #"@org_tensorflow//tensorflow:linux_ppc64le": ["-lrt"],
        "@platforms//os:linux": ["-lrt"],
        "//conditions:default": [],
    }),
    textual_hdrs = _TEXTUAL_HDRS_LIST,
    visibility = ["//visibility:public"],
    deps = [":onednn_autogen"],
    #+ if_mkl_ml(
    #    ["@org_tensorflow//third_party/mkl:intel_binary_blob"],
    #    [],
    #),
)
