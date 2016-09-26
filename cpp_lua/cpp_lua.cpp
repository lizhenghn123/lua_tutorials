#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <iostream>

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

#define err_exit(num,fmt, ...)  \
    do{ printf("[%s:%d]" fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); return (num); } while(0)

// lua中调用的c函数定义,实现乘法
// lua 调用C接口时，其每一个接口都是此形式，而具体的参数lua_pushXXX和lua_toXXX进行传递；luaState*所指的结构中包含了Lua调用此Dll时必备的Lua环境
#ifdef __cplusplus 
extern "C"
#endif
int cmul(lua_State *l) 
{
    int a = lua_tointeger(l, 1);
    int b = lua_tointeger(l, 2);
    lua_pushinteger(l, a * b);
    return 1;
}

int main(int argc, char **argv)
{
    //err_exit(-1,"luaL_newstat() failed");

    // step 1
    lua_State *lua = luaL_newstate();       //创建lua运行环境
    if(lua == NULL) err_exit(-1, "luaL_newstat() failed");

    int ret = 0;

    // step 2
    ret = luaL_loadfile(lua, "cpp_lua.lua");   //加载lua脚本文件
    if(ret != 0) err_exit(-1, "luaL_loadfile(cpp_lua.lua) failed");
    ret = lua_pcall(lua, 0, 0, 0) ;
    if(ret != 0) err_exit(-1, "lua_pcall failed:%s", lua_tostring(lua, -1));

    // case 1 ： 获取lua中定义的变量
    lua_getglobal(lua, "width");            //获取lua中定义的变量
    lua_getglobal(lua, "height");
    lua_getglobal(lua, "length");
    printf("length:%ld, height:%ld, width:%ld\n\n", lua_tointeger(lua, -1), lua_tointeger(lua, -2), lua_tointeger(lua, -3));
    lua_pop(lua, 3);                        //恢复lua的栈, 压栈是3次，pop也要3次

    // case 2 ： 调用lua中定义的函数sum
    // 为了在c/c++中调用lua中的函数，首先需要把lua的函数压入堆栈，
	// 然后把函数的参数压入堆栈，然后执行函数，最后从栈中获取函数返回值。
    int a = 11;
    int b = 12;
    lua_getglobal(lua, "sum");              //调用lua中定义的函数sum
    lua_pushinteger(lua, a);
    lua_pushinteger(lua, b);
    ret = lua_pcall(lua, 2, 1, 0);          //表示2个参数一个返回值

    if(ret != 0) err_exit(-1, "lua_pcall failed:%s", lua_tostring(lua, -1));
    printf("sum:%d + %d = %ld\n\n", a, b, lua_tointeger(lua, -1));

    lua_pop(lua, 1);                        //恢复lua的栈

    // case 3 ：调用lua中的函数mystrcat
    const char str1[] = "hello";
    const char str2[] = "world";
    lua_getglobal(lua, "mystrcat");         //调用lua中的函数mystrcat
    lua_pushstring(lua, str1);
    lua_pushstring(lua, str2);
    ret = lua_pcall(lua, 2, 1, 0);          //表示2个参数一个返回值
    if(ret != 0) err_exit(-1, "lua_pcall failed:%s", lua_tostring(lua, -1));
    printf("mystrcat:%s%s = %s\n\n", str1, str2, lua_tostring(lua, -1));
    lua_pop(lua, 1);                        //恢复lua的栈

    // case 4 ：注册cmul到lua环境中，调用lua接口，再有lua接口反向调用cmul实现
    lua_pushcfunction(lua, cmul);           //将C语言定义的cmul函数注册到lua中, 以在lua中使用
    lua_setglobal(lua, "cmul");             //绑定到lua中的名字cmul

    lua_getglobal(lua, "luamul");            //调用lua中的luamul函数，该函数调用本程序中定义的cmul函数实现乘法
    lua_pushinteger(lua, a);
    lua_pushinteger(lua, b);
    ret = lua_pcall(lua, 2, 1, 0);
    if(ret != 0) err_exit(-1, "lua_pcall failed:%s", lua_tostring(lua, -1));
    printf("mul: %d * %d = %ld\n\n", a, b, lua_tointeger(lua, -1));
    lua_pop(lua, 1);

    // case 5 ：调用lua接口，且返回多个值
    {
        const char str1[] = "hello";
        const char str2[] = "world";
        int a = 23, b = 45;
        lua_getglobal(lua, "multireturn");         //调用lua中的函数multireturn
        lua_pushinteger(lua, a);
        lua_pushinteger(lua, b);
        lua_call(lua, 2, 4);                       //表示2个参数4个返回值
        
        std::cout << "multireturn:\n";
        while(lua_gettop(lua))
        {
            std::cout << "\t";
            switch(lua_type(lua, lua_gettop(lua)))
            {
                case LUA_TNUMBER:
                    std::cout << "lua returned the number: " << lua_tonumber(lua, lua_gettop(lua));
                    break;
                case LUA_TTABLE:
                    std::cout << "lua returned a table";
                    //std::cout << "lua returned a table: " << lua_tostring(lua, lua_gettop(lua));
                    break;
                case LUA_TSTRING:
                    std::cout << "lua returned the string: " << lua_tostring(lua, lua_gettop(lua));
                    break;
                case LUA_TBOOLEAN:
                    std::cout << "lua returned the boolean: " << lua_toboolean(lua, lua_gettop(lua));
                    break;
                default:
                    std::cout << "lua returned an unknown-type value";
            }
            lua_pop(lua, 1);
            std::cout << "\n";            
        }

        lua_pop(lua, 4);                           //恢复lua的栈    
    }

    // step 3
    lua_close(lua);                                //释放lua运行环境
    return 0;
}
