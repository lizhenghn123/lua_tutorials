#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <string>

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

// lua中调用的c函数定义,实现乘法和减法
// lua 调用C接口时，其每一个接口都是此形式，而具体的参数lua_pushXXX和lua_toXXX进行传递；luaState*所指的结构中包含了Lua调用此Dll时必备的Lua环境
extern "C" int cmul_sub(lua_State *l) 
{
    int a = lua_tointeger(l, 1);    // 如果传进来的是浮点，会截断，如果传进来的是字符串，会按整数0处理
    int b = lua_tointeger(l, 2);
    printf("cmul_sub get params: %d, %d\n", a, b);
    lua_pushinteger(l, a * b);
    lua_pushinteger(l, a - b);
    return 2;   // 返回值有2个
}

// lua中调用的c函数定义,实现字符串拼接
extern "C" int ccat(lua_State *l) 
{
    std::string a = lua_tostring(l, 1);
    const char* b = lua_tostring(l, 2);
    const char* c = lua_tostring(l, 3);
    a = a + "_" + b + "_" + c;
    lua_pushstring(l, a.c_str());
    return 1;   // 返回值只有一个
}

int main(int argc, char **argv)
{
    // step 1
    lua_State *lua = luaL_newstate();       //创建lua运行环境
    if(lua == NULL) err_exit(-1, "luaL_newstat() failed");

    int ret = 0;

    // step 2
    ret = luaL_loadfile(lua, "subfunc.lua");   //加载lua脚本文件
    if(ret != 0) err_exit(-1, "luaL_loadfile failed");
    ret = lua_pcall(lua, 0, 0, 0) ;
    if(ret != 0) err_exit(-1, "lua_pcall failed:%s", lua_tostring(lua, -1));


    // 将本程序定义的几个接口注册给lua运行时环境
    lua_pushcfunction(lua, cmul_sub);           //将C语言定义的cmul_sub函数注册到lua中, 以在lua中使用
    lua_setglobal(lua, "cmul_sub");             //绑定到lua中的名字cmul_sub

    lua_pushcfunction(lua, ccat);               //将C语言定义的ccat函数注册到lua中, 以在lua中使用
    lua_setglobal(lua, "ccat");                 //绑定到lua中的名字ccat

    {   // test cmul_sub
        int a = 10, b = 3;
        lua_getglobal(lua, "luamul_sub");       //调用lua中的luamul_sub函数，该函数调用本程序中定义的cmul函数实现乘法和减法
        lua_pushinteger(lua, a);
        lua_pushinteger(lua, b);
        ret = lua_pcall(lua, 2, 2, 0);          // 2个参数，2个返回值
        if(ret != 0) err_exit(-1, "lua_pcall failed:%s, %s", lua_tostring(lua, -1), lua_tostring(lua, -2));
        // -1 及N(返回值个数) 表示栈顶; -N(返回值个数) 及 1 表示栈底
        printf("luamul_sub: %d * %d = %ld, %d - %d = %d\n\n", a, b, lua_tointeger(lua, -2), a, b, lua_tointeger(lua, -1));
        printf("luamul_sub: %d * %d = %ld, %d - %d = %d\n\n", a, b, lua_tointeger(lua, 1), a, b, lua_tointeger(lua, 2));
        lua_pop(lua, 2);   // 2 个返回值，所以栈要弹出2次
    }
    {   // test cmul_sub : 测试传递浮点数和字符串值并进行乘法和减法运算
        float d1 = 12.34;
        const char* d2 = "hello";
        lua_getglobal(lua, "luamul_sub");       //调用lua中的luamul_sub函数，该函数调用本程序中定义的cmul函数实现乘法和减法
        lua_pushnumber(lua, d1);
        lua_pushstring(lua, d2);
        ret = lua_pcall(lua, 2, 2, 0);          // 2个参数，2个返回值
        if(ret != 0) err_exit(-1, "lua_pcall failed:%s, %s", lua_tostring(lua, -1), lua_tostring(lua, -2));
        // -1 及N(返回值个数) 表示栈顶; -N(返回值个数) 及 1 表示栈底
        printf("luamul_sub: %f * %s = %ld, %f - %s = %d\n\n", d1, d2, lua_tointeger(lua, -2), d1, d2, lua_tointeger(lua, -1));
        lua_pop(lua, 2);   // 2 个返回值，所以栈要弹出2次
    }

    {   // test ccat
        int a = 111, b = 232;
        lua_getglobal(lua, "luacat");           //调用lua中的luacat函数，该函数调用本程序中定义的ccat函数实现字符串拼接
        lua_pushinteger(lua, a);
        lua_pushinteger(lua, b);
        lua_pushinteger(lua, 1234);
        ret = lua_pcall(lua, 3, 1, 0);          // 3个参数，1个返回值
        if(ret != 0) err_exit(-1, "lua_pcall failed:%s", lua_tostring(lua, -1));
        printf("luacat: %d, %d, %d = %s\n\n", a, b, 1234, lua_tostring(lua, 1));
        lua_pop(lua, 1);                        // 1 个返回值，所以栈要弹出1次
    }
    {   // test ccat : 测试压入不同类型的数据然后进行拼接: 字符串、浮点型、整形
        const char* c1 = "hello"; 
        float c2 = 12.5678; 
        int c3 = 123456; 
        lua_getglobal(lua, "luacat");           //调用lua中的luacat函数，该函数调用本程序中定义的ccat函数实现字符串拼接
        lua_pushstring(lua, c1);
        lua_pushnumber(lua, c2);
        lua_pushinteger(lua, c3);
        ret = lua_pcall(lua, 3, 1, 0);          // 3个参数，1个返回值
        if(ret != 0) err_exit(-1, "lua_pcall failed:%s", lua_tostring(lua, -1));
        printf("luacat: %s, %f, %d = %s\n\n", c1, c2, c3, lua_tostring(lua, 1));
        lua_pop(lua, 1);                        // 1 个返回值，所以栈要弹出1次
    }    

    // step 3
    lua_close(lua);                         //释放lua运行环境
    return 0;
}
