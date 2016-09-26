#include <iostream>
#include <string>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
//#pragma comment(lib, "lua.lib")
};

using namespace std;

lua_State* initLuaEnv()
{
	lua_State* luaEnv = lua_open();
	luaopen_base(luaEnv);
	luaL_openlibs(luaEnv);
	return luaEnv;
}


// 加载Lua文件到Lua运行时环境中.
bool loadLuaFile(lua_State* luaEnv, const string& fileName)
{
	int result = luaL_loadfile(luaEnv, fileName.c_str());
	if (result)
	{
		return false;
	}
	
	result = lua_pcall(luaEnv, 0, 0, 0);
	return result == 0;
}

// 获取全局函数.
lua_CFunction getGlobalProc(lua_State* luaEnv, const string& procName)
{
	lua_getglobal(luaEnv, procName.c_str());
	if (!lua_iscfunction(luaEnv, 1))
	{
		return 0;
	}

	return lua_tocfunction(luaEnv, 1);
}

int main()
{
	// 初始化Lua运行时环境.
	lua_State* luaEnv = initLuaEnv();
	if (!luaEnv)
	{
		return -1;
	}

	// 加载脚本到Lua环境中.
	if (!loadLuaFile(luaEnv, "generateXML.lua"))
	{
		cout<<"Load Lua File FAILED!"<<endl;
		return -1;
	}

	// 获取信息.
	string fromName("beijing");
	string toName("shanghai");
	string msgContent("how many miles from these tow cities?");

	// 将要调用的函数和函数调用参数入栈.
	// 为了在c/c+=中调用lua中的函数，首先需要把lua的函数压入堆栈，
	// 然后把函数的参数压入堆栈，然后执行函数，最后从栈中获取函数返回值。
	lua_getglobal(luaEnv, "generateNoteXML");
	lua_pushstring(luaEnv, fromName.c_str());
	lua_pushstring(luaEnv, toName.c_str());
	lua_pushstring(luaEnv, msgContent.c_str());

	// 调用Lua函数（3个参数,一个返回值）.
	lua_pcall(luaEnv, 3, 1, 0);

	// 获取返回值.
	if (lua_isboolean(luaEnv, -1))
	{
		int success = lua_toboolean(luaEnv, -1);
		if (success)
		{
			cout<<"\nGenerate Note File Successful!"<<endl;
		}
	}
	
    // 将返回值出栈.
	lua_pop(luaEnv, 1);   
	
    // 释放Lua运行时环境.
	lua_close(luaEnv);   
	return 0;
}
