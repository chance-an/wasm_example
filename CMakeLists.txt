cmake_minimum_required(VERSION 3.10)

project(WasmApp)

if(EMSCRIPTEN)
    include(cmake/wasm-app.cmake)
else()
    # 使用native调试？
endif()
