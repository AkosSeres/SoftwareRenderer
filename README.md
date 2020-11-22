# WASMRenderBench
A benchmark between WebAssembly, JavaScript and native C software rendering in SDL.
The WASM build is available [here](https://akosseres.github.io/WASMRenderBench/)! When opened, it loads a base scene from a prepackaged .obj file, but any [Wavefront .obj file](https://en.wikipedia.org/wiki/Wavefront_.obj_file) can be opened and viewed by dragging and dropping the .obj file into the browser window on the canvas.
##### Controls:
 - WASD: movement
 - Space: go up
 - Left shift: go down
 - Escape: release mouse lock
# Building
The same codebase is used across all build targets, with small differences between them. For the native build, CMake is used and for WASM there is a separate `build_wasm.sh` build script.
### WASM
Emscripten is used to compile and build the WASM target so the latest version of *[emsdk](https://emscripten.org/docs/getting_started/downloads.html)* should be installed and included in `PATH`. Can be built by running the script:
```
sh build_wasm.sh
```
After building the directory `dest` will contain the compiled target.
### Linux/Unix
CMake (`sudo apt install cmake` on Ubuntu...) and SDL2 (`sudo apt install libsdl2-dev`) has to be installed before building, then build with:
```
mkdir build
cd build
cmake ..
make
```
The executable should be ready in the build directory along with the base_scene.obj file.
### Windows
On Windows, the CMake GUI should be downloaded along with the latest version of [SDL2](https://www.libsdl.org/download-2.0.php), then after configuring the install folder of SDL2 in CMake, the Makefile can be generated.