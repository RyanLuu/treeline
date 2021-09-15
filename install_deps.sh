#!/usr/bin/env bash

rm -rf src/lib/*

git clone -b dev-0.11 https://github.com/mackron/miniaudio.git src/lib/miniaudio

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

