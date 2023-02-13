# gmod_longjmp


Escaping lua/C/C++ stack frames. 

DOES NOT WORK. 

For reference only what does not work.

```bash
srcds@meta3:~/compiling_for_gmod_x64/gmod_longjmp$ ./ci.sh
Setting up watches.  Beware: since -r was given, this may take a while!
Watches established.
src/ MODIFY gmlongjmp.cpp


Building configurations...
Running action 'gmake'...
Generated solutions/linux-gmake/Makefile...
Generated solutions/linux-gmake/longjmp.make...
Done (33ms).
==== Building longjmp (release_x86_64) ====
Creating obj/x86_64
mkdir -p obj/x86_64
gmlongjmp.cpp
g++   -MMD -MP -DDEBUG -DLINUX -I/usr/include/libunwind -I/usr/include/luajit-2.1 -I/usr/include/x86_64-linux-gnu/c++/10 -I/usr/include/c++/10 -I../../src -m64 -ffast-math -O0 -fPIC -g -msse -std=c++11 -fPIC -fpermissive  -o "obj/x86_64/gmlongjmp.o" -MF "obj/x86_64/gmlongjmp.d" -c "../../src/gmlongjmp.cpp"
main.cpp
g++   -MMD -MP -DDEBUG -DLINUX -I/usr/include/libunwind -I/usr/include/luajit-2.1 -I/usr/include/x86_64-linux-gnu/c++/10 -I/usr/include/c++/10 -I../../src -m64 -ffast-math -O0 -fPIC -g -msse -std=c++11 -fPIC -fpermissive  -o "obj/x86_64/main.o" -MF "obj/x86_64/main.d" -c "../../src/main.cpp"
Linking longjmp
g++ -o "../../out/linux/gmsv_longjmp_linux64.dll" obj/x86_64/gmlongjmp.o obj/x86_64/main.o    -L../../lib64/linux -L/usr/lib64 -m64 -shared -Wl,-soname=gmsv_longjmp_linux64.dll  -fPIC -lunwind -liberty -lpthread -lrt
===TESTING gmod_longjmp===
lua: INFO       LuaJIT 2.1.0-beta3      x64     true    SSE3    SSE4.1  fold    cse     dce     fwd     dse     narrow  loop    abc     sink    fuse
lua: Calling infinite loop

longjmp:longjmp.timed() timed out!
lua: pcall of longjmp.timed returned:   true    false
lua: Calling jitted infinite loop

longjmp:longjmp.timed() timed out!
lua: pcall of longjmp.timed returned:   true    false
lua: Calling complex function
longjmp: TEST: class constructor called!

longjmp:longjmp.timed() timed out!
lua: pcall of longjmp.timed returned:   true    false
longjmp: TEST: class constructor called! MEMORY LEAK: NEVER DESTRUCTED!!!!

longjmp:longjmp.timed() timed out!
lua: pcall of longjmp.timed returned:   true    false
lua: EOF
luajit returned 0
Setting up watches.  Beware: since -r was given, this may take a while!
Watches established.

```
