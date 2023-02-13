#!/bin/bash

while true; do

        inotifywait -e modify,create,delete -r src premake5.lua && sleep 1 && \
	(rm solutions -rf;premake5 gmake && cd solutions/linux-gmake/ && make config=release_x86_64 verbose=1 ) \
		 && ( cp out/linux/gmsv_longjmp_linux64.dll longjmp.so && patchelf --add-needed lua_shared.so out/linux/gmsv_longjmp_linux64.dll && ( luajit payload.lua;echo "luajit returned $?" ) ); done

done

