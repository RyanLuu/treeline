#!/usr/bin/env bash

curl -L --output mpg123.tar.bz2 https://sourceforge.net/projects/mpg123/files/latest/download
tar -xf mpg123.tar.bz2
rm mpg123.tar.bz2
cd mpg123-*
./configure
make src/libmpg123/libmpg123.la
cp src/libmpg123/.libs/libmpg123.dylib /usr/local/lib
cd ..
rm -r mpg123-*

sdl_deps=(SDL SDL_image SDL_mixer SDL_ttf)
for dep in $sdl_deps; do
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

