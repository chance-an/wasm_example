#!/usr/bin/env bash
BASE_DIR=`git rev-parse --show-toplevel`

if [[ ! -d $BASE_DIR/emsdk ]]; then
    # doc：https://emscripten.org/docs/getting_started/downloads.html
    echo "install emsdk"
    git submodule update --init --recursive
    cd emsdk
    ./emsdk install latest
    ./emsdk activate latest
else
    echo "emsdk already installed"
    . $BASE_DIR/emsdk/emsdk_env.sh
fi

