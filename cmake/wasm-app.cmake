include(cmake/common.cmake)

# lib use cmake?
# add_subdirectory(lib)
set(EXAMPLE_LINK_OPTIONS
    ${COMMON_FLAGS}
    -sLLD_REPORT_UNDEFINED
    -sALLOW_MEMORY_GROWTH=1
    -sFORCE_FILESYSTEM=0
    -sFILESYSTEM=0
    -sNO_EXIT_RUNTIME=1
    -sINITIAL_MEMORY=128MB
    -sWASM=1
    -sUSE_WEBGL2=1
    -sDEMANGLE_SUPPORT=1
    -sASSERTIONS=1
    -sGL_ASSERTIONS=1
    -sUSE_SDL=2
    -sSAFE_HEAP=1
    -sTOTAL_MEMORY=33554432
    -sMODULARIZE=0 
    -sNO_EXIT_RUNTIME=1 
    -sNO_DYNAMIC_EXECUTION=1
    -sFETCH
    -sUSE_ZLIB=1 
    -fexceptions
)

set(EXAMPLE_INCLUDE_DIRECTORIES
    .
    ${PROJECT_BINARY_DIR}
    ${COMMON_INCLUDE_DIRECTORIES}
)

add_executable(wasm-example
    ${SRCS}
)

target_link_libraries(wasm-example PUBLIC
)

# add_definitions()
add_custom_command(TARGET wasm-example POST_BUILD

    # 好像是有什么bug，导致后缀是js，而不是html，先直接改名？
    COMMAND ditto ${CMAKE_BINARY_DIR}/wasm-example.js ${CMAKE_SOURCE_DIR}/example_web/index.html
    COMMAND ditto ${CMAKE_BINARY_DIR}/wasm-example_.js ${CMAKE_SOURCE_DIR}/example_web/
    COMMAND ditto ${CMAKE_BINARY_DIR}/wasm-example.wasm ${CMAKE_SOURCE_DIR}/example_web/
)

target_compile_options(wasm-example PUBLIC
    ${COMMON_FLAGS}
    "-I${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}"
)

target_include_directories(wasm-example PUBLIC
    ${EXAMPLE_INCLUDE_DIRECTORIES}
)

target_link_options(wasm-example PUBLIC
    ${EXAMPLE_LINK_OPTIONS}
    --oformat=html

    # if only modify cpp file
    # --oformat=bare
)