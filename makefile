#!make

# This file takes reference from https://github.com/mdaines/viz.js/blob/master/Makefile

EMCC=emcc
GRAPHVIZ_DIR=lib/graphviz-7.0.5

LIBOPTS=-O0 -g
LIB_DEST=$(abspath lib/graphviz-7.0.5)

EMCC_OPT=-s SAFE_HEAP=1

default: main

clean:

clobber: | clean
	rm -rf $(LIB_DEST)

# $(LIB_DEST):
# 	mkdir -p $(LIB_DEST)

# build/stage/graphviz: FORCE
# 	mkdir -p $@
# 	tar -zxf lib/graphviz-7.0.5.tar.gz --strip-components 1 -C $@

# graphviz:
# 	# cd build/stage/graphviz && ./configure --quiet
# 	# cd build/stage/graphviz && emconfigure ./configure --quiet --without-sfdp --disable-ltdl --enable-static --disable-shared --prefix=$(LIB_DEST) --libdir=$(LIB_DEST)/lib CFLAGS="-Oz -w"
# 	cd build/stage/graphviz && emmake make --quiet lib plugin
# 	cd build/stage/graphviz/lib && emmake make --quiet install
# 	cd build/stage/graphviz/plugin && emmake make --quiet install


# graphviz_wasm: 
# 	$(EMCC) -std=c++14 -s TOTAL_MEMORY=33554432 -s ALLOW_MEMORY_GROWTH=1 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -s WASM=1 -s USE_SDL=2 --memory-init-file 0 -sMAX_WEBGL_VERSION=2 -s USE_ZLIB=1 -s MODULARIZE=0 -s LEGACY_VM_SUPPORT=1 -s NO_DYNAMIC_EXECUTION=1 -g -sFETCH -s NO_EXIT_RUNTIME=1 -fexceptions -o build/graph.html -Ilib/json -I$(LIB_DEST)/include -I$(LIB_DEST)/include/graphviz -L$(LIB_DEST)/lib -L$(LIB_DEST)/lib/graphviz -lgvplugin_core -lgvplugin_dot_layout -lgvplugin_neato_layout -lcdt -lcgraph -lgvc -lgvpr -lpathplan -lxdot src/graphviz.cpp src/opengl.cpp

main: 
	$(EMCC) -std=c++14 -s TOTAL_MEMORY=33554432 -s ALLOW_MEMORY_GROWTH=1 -s USE_SDL_IMAGE=2 -s WASM=1 -s USE_SDL=2 --memory-init-file 0 -sMAX_WEBGL_VERSION=2 -s MODULARIZE=0 -s LEGACY_VM_SUPPORT=1 -g -s NO_EXIT_RUNTIME=1 -fexceptions -o build/graph.html -Ilib/json src/main.cpp

test:
	echo $@

FORCE:
