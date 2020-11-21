mkdir dest obj
cp ./index.html ./dest/index.html
emcc -c src/main.c -o obj/main.o -I include -s USE_SDL=2
emcc -c src/ccanvas.c -o obj/ccanvas.o -I include -s USE_SDL=2
emcc -c src/camera.c -o obj/camera.o -I include -s USE_SDL=2
emcc -c src/point.c -o obj/point.o -I include -s USE_SDL=2
emcc -c src/scene.c -o obj/scene.o -I include -s USE_SDL=2
emcc -c src/vec3.c -o obj/vec3.o -I include -s USE_SDL=2
emcc obj/main.o obj/ccanvas.o obj/camera.o obj/point.o obj/scene.o obj/vec3.o -o dest/index.js -s USE_SDL=2 -s EXPORTED_FUNCTIONS='["_CCanvas_dropEventForSDL","_main"]' -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]'