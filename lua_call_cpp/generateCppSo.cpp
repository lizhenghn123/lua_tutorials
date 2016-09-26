#include <stdio.h>
#include <string>
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

// 发生错误时报告错误.  
void reportError(lua_State* luaEnv, const char* msg)  
{  
    lua_pushstring(luaEnv, msg);  
    lua_error(luaEnv);  
}  

// 检测函数调用参数个数是否正常
void checkArgsCount(lua_State* luaEnv, int paramCount)
{
    // lua_gettop获取栈中元素个数.  
    if (lua_gettop(luaEnv) != paramCount)  
    {  
        reportError(luaEnv, "error : num of args");  
    }  
}

#ifdef __cplusplus 
extern "C"
#endif
int cpp_mul(lua_State *l)
{
    checkArgsCount(l, 2);

    int a = lua_tointeger(l, 1);
    int b = lua_tointeger(l, 2);
    lua_pushinteger(l, a * b);
    return 1;    // 返回值个数为1个
} 

#ifdef __cplusplus 
extern "C"
#endif
int cpp_cat(lua_State *l)
{
    checkArgsCount(l, 2);

    std::string a = lua_tostring(l, 1);
    std::string b = lua_tostring(l, 2);
    std::string r(a + "_" + b);
    lua_pushstring(l, r.c_str());
    return 1;    // 返回值个数为1个
} 


#ifdef __cplusplus 
extern "C"
#endif
int cpp_print_int_string_float(lua_State *l)
{
    checkArgsCount(l, 3);

    int a = lua_tointeger(l, 1);
    std::string b = lua_tostring(l, 2);
    float c = lua_tonumber(l, 3);

    printf("cpp_print_int_string_float : %d %s %f\n", a, b.c_str(), c);
    //lua_pushstring(l, a + "_" + b);
    return 0;    // 返回值个数为0个
} 


// 导出函数列表.  
static luaL_Reg luaLibs[] =  
{  
    {"cpp_mul", cpp_mul},  
    {"cpp_cat", cpp_cat},  
    {"cpp_print_int_string_float", cpp_print_int_string_float},  
    {NULL, NULL}  
};  
  
// 动态库入口函数，lua调用此入口函数. 入口函数名必须是luaopen_{动态库文件名,除去后缀.so}
extern "C" 
int luaopen_libgenerateCppSo(lua_State* luaEnv)  
{  
    const char* LIBRARY_NAME = "libgenerateCppSo";
    luaL_register(luaEnv, LIBRARY_NAME, luaLibs);  // luaL_register 在lua5.2版本上已经不可使用，可以使用：luaL_requiref及luaL_newlib
  
    return 1;  
}  