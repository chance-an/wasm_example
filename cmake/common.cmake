
set(CMAKE_CXX_STANDARD 20)

get_filename_component(EXAMPLE_ROOT "${CMAKE_CURRENT_LIST_DIR}/../" ABSOLUTE)

execute_process(COMMAND which emcc OUTPUT_VARIABLE EMCC_PATH)

get_filename_component(EMSDK_LOCATION "./../../../" REALPATH BASE_DIR ${EMCC_PATH})

message(STATUS "EMSDK_LOCATION: ${EMSDK_LOCATION}")

set(SRCS
    ${EXAMPLE_ROOT}/src/main.cpp
)

message(STATUS "${SRCS}")

set(COMMON_FLAGS
    -std=c++20
    -Werror=return-type
)

if(EXAMPLE_BUILD_TYPE MATCHES Release)
    set(COMMON_FLAGS
        ${COMMON_FLAGS}
        -O3
    )
else()
    set(COMMON_FLAGS
        ${COMMON_FLAGS}
        -O0
        -g3
        -DDEBUG
    )
endif()

# include default search path which founded in emsdk/bazel/emscripten_toolchain/toolchain.bzl

# "emscripten/cache/sysroot/include/c++/v1/**",
# "emscripten/cache/sysroot/include/compat/**",
# "emscripten/cache/sysroot/include/**",
# "lib/clang/16/include/**",
set(COMMON_INCLUDE_DIRECTORIES
    ${EMSCRIPTEN_SYSROOT}/include/c++/v1
    ${EMSCRIPTEN_SYSROOT}/include/compat
    ${EMSDK_LOCATION}/upstream/lib/clang/16/include

    # 不知道什么原因，这里设置不上（compile_commands.json找不到，必须在compile选项里面直接用-I设置）
    # ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}
)
message(STATUS "${COMMON_INCLUDE_DIRECTORIES}")