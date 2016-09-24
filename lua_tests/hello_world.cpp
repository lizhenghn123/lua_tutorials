#include <stdio.h>

//lua头文件
#ifdef __cplusplus 
extern "C"
{
#endif
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
#ifdef __cplusplus 
}
#endif

int main()
{
    lua_State * state = luaL_newstate();
    luaL_openlibs(state);
    luaL_dostring(state, "print(\"hello lua, by cpp\")");
    lua_close(state);
}
