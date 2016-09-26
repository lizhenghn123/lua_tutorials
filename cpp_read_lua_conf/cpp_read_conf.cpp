#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <inttypes.h>
#include "lua.hpp"
using namespace std;

#define err_exit(num,fmt, ...)  \
    do{ printf("[%s:%d]" fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); return (num); } while(0)

const char* filename = "config.lua";

int luaConfig(lua_State* lua)
{
    int ret = luaL_loadfile(lua, filename);   //加载lua脚本文件
    if(ret != 0) err_exit(-1, "luaL_loadfile(cpp_lua.lua) failed");
    ret = lua_pcall(lua, 0, 0, 0) ;
    if(ret != 0) err_exit(-1, "lua_pcall failed:%s", lua_tostring(lua, -1));

    lua_getglobal(lua, "width");            //获取lua中定义的变量
    lua_getglobal(lua, "height");
    lua_getglobal(lua, "length");
    printf("length:%ld, height:%ld, width:%ld\n", lua_tointeger(lua, -1), lua_tointeger(lua, -2), lua_tointeger(lua, -3));
    lua_pop(lua, 3);                        //恢复lua的栈, 压栈是3次，pop也要3次
}

int fileread()
{
    FILE* f = fopen(filename, "r");
    long size = ftell(f);
    char buffer[1024];
    int n = fread(buffer, 1, 1024, f);
    buffer[n] = '\0';
    printf("fileread: %s\n", buffer);
}

int64_t getNow()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec * 1000 * 1000 + now.tv_usec;
} 

int main(int argc, char **argv)
{
    lua_State *lua = luaL_newstate();       //创建lua运行环境
    if(lua == NULL) err_exit(-1, "luaL_newstat() failed");

    while(true)
    {
        int64_t start = getNow();
        luaConfig(lua);
        int64_t end = getNow();
        printf("lua read elapsed : %lld us\n\n", end - start);

        start = getNow();
        fileread();
        end = getNow();
        printf("file read elapsed : %lld us\n\n", end - start);

        sleep(1);    // 等待外部修改配置文件，这里验证是否能否读取新值：可以
    }
}