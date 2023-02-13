#include "lua.hpp"


	
extern "C" int
luaopen_longjmp(lua_State *L);

#ifdef _WIN32
	#define GLUA_DLL_EXPORT  __declspec( dllexport ) 
#else
	#define GLUA_DLL_EXPORT	 __attribute__((visibility("default"))) 
#endif

extern "C" GLUA_DLL_EXPORT int gmod13_open( lua_State* L )
{
	luaopen_longjmp(L);
	lua_setglobal(L, "longjmp");
	return 0;
}

extern "C" GLUA_DLL_EXPORT int gmod13_close( lua_State* L )
{
	return 0;
}


