mkdir test/bin

gcc test/vec3_test.c src/vec3.c -o test/bin/vec3_test -Iinclude/ -Itest/ -lm
./test/bin/vec3_test

rm -rf test/bin