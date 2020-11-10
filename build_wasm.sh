mkdir dest obj
emcc -c src/main.c -o obj/main.o -I include -s USE_SDL=2
emcc -c src/ccanvas.c -o obj/ccanvas.o -I include -s USE_SDL=2
emcc obj/main.o obj/ccanvas.o -o dest/index.html -s USE_SDL=2