#!/usr/bin/env bash

curl https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h >> src/lib/miniaudio.h

sdl_deps=(SDL SDL_image SDL_ttf)
for dep in "${sdl_deps[@]}"; do
	git clone https://github.com/libsdl-org/$dep
	cd $dep
	mkdir build
	cd build
	../configure
	make
	sudo make install
	cd ../..
	rm -rf $dep
done

