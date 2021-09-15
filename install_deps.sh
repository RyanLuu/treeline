#!/usr/bin/env bash

rm -rf src/lib/*
mkdir -pv src/lib/miniaudio/research
curl https://raw.githubusercontent.com/mackron/miniaudio/dev-0.11/research/miniaudio_engine.h > src/lib/miniaudio/research/miniaudio_engine.h
curl https://raw.githubusercontent.com/mackron/miniaudio/dev-0.11/miniaudio.h > src/lib/miniaudio/miniaudio.h

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

