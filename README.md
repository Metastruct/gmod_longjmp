# gmod_longjmp

**Purpose/idea:** for sandboxing code (in-process, time-limiting)

*What if one day we could have compiled/jitted code we can escape from with a timer/signal*

**DOES NOT WORK.**

For reference only on what does NOT work (likely nothing will since we call out of untrusted into almost arbitrary complexity functions that might for example allocate FDs or memory).

## Alternative approaches

### 1. Assumption: Any external C functions are well below any timeout limits or a hard timeout limit is not required

Required modifications: [LUAJIT_ENABLE_CHECKHOOK](https://github.com/LuaJIT/LuaJIT/blob/master/src/lj_record.c#L2239) or some way to patch the presently running code to contain the check if jitted (*undefined computation behaviour of escaping loops prematurely and such needs to be tolerated but this would happen anyway with sethook*)

Are you able to restore LuaJIT to a functional state from a jitted code such as `while true do end`?
Assuming the above and that any functions the untrusted code can execute does not dominate:
 - on timeout, restore if within lua
 - if calling a C function: breakpoint/callback on resuming lua execution at which point execution would be possible again
  
**Problems:** nested lua:
1. Enter timeout function:
2. Run `ply:Kill()` 
3. C runs Lua
4. `hook.Add("OnPlayerDying","",function() ` timing out here is not possible unless we can peel this frame separately and then kill part 2. separately after we are back in lua ` end)`

### 2. Copy-on-write approach

In-process checkpointing of memory allocations/stack/etc and other modifications and restore fully to previous state on timeout and close any new handles and such. Similar to DMTCP.

fork: 8ms, too slow
vfork: similar to longjmp so same problems arise with state getting mangled.

### 3. Separate lua state with faked api environment that calls back to the actual state

On timeout, discard the whole state. It wasn't calling anything C anyway.


### 4. Solutions/issues elsewhere
 - [v8 Javascript isolate::TerminateExecution()](https://v8docs.nodesource.com/node-4.8/d5/dda/classv8_1_1_isolate.html#ab33b5ea0fbd412614931733449c3d659)
 - https://github.com/tarantool/tarantool/issues/1898
 - https://github.com/LuaJIT/LuaJIT/issues/723#issuecomment-862662396
 - https://github.com/WebAssembly/design/issues/1380

## Test output (works sometimes (undefined behaviour!))

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


```bash
...

lua: Calling complex function

longjmp:longjmp.timed() timed out!
lua: pcall of longjmp.timed returned:             true                false

[New Thread 0x7fffd88376c0 (LWP 852962)]
[New Thread 0x7fffd87366c0 (LWP 852963)]

Thread 1 "srcdscustom" received signal SIGSEGV, Segmentation fault.
0x00007fffd2e696c5 in ?? () from /home/srcds/Steam/steamapps/common/GarrysModDS/bin/linux64/lua_shared.so
(gdb) bt
#0  0x00007fffd2e696c5 in ?? () from /home/srcds/Steam/steamapps/common/GarrysModDS/bin/linux64/lua_shared.so
#1  0x00007fffd2e5b8a2 in lua_pcall () from /home/srcds/Steam/steamapps/common/GarrysModDS/bin/linux64/lua_shared.so
#2  0x00007ffff7fb3a44 in longjmp_timed (L=0x7ffff6d5e378) at ../../src/gmlongjmp.cpp:221
#3  0x00007fffd2e69865 in ?? () from /home/srcds/Steam/steamapps/common/GarrysModDS/bin/linux64/lua_shared.so
#4  0x00007fffd2e5b8a2 in lua_pcall () from /home/srcds/Steam/steamapps/common/GarrysModDS/bin/linux64/lua_shared.so
#5  0x00007fffd2e1d18c in ?? () from /home/srcds/Steam/steamapps/common/GarrysModDS/bin/linux64/lua_shared.so
#6  0x00007fffd2e25655 in ?? () from /home/srcds/Steam/steamapps/common/GarrysModDS/bin/linux64/lua_shared.so
...
```
