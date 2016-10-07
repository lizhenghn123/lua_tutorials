#include "Student.h"
#include <stdio.h>
#include "lua.hpp"

const char *CLASS_NAME = "Student";

int newStudent( lua_State *l )
{
	//const char*name = lua_tostring(l,-1);
	//Student *a = new Student(name);
	Student *a = new Student();  //这里我们分配了内存，后面我们会介绍怎么让Lua在gc的时候释放这块内存
	void **p = (void**)lua_newuserdata( l, sizeof(void*));  
	*p = a;

	luaL_getmetatable( l, CLASS_NAME );
	lua_setmetatable( l, -2 );
	return 1;
}

Student* getStudent( lua_State *l )
{
	luaL_checktype( l, 1, LUA_TUSERDATA ); //indicate what type to check
	void *ud = luaL_checkudata( l, 1, CLASS_NAME );
	if( !ud )
		luaL_typerror( l, 1, CLASS_NAME );

	return *(Student**)ud;
}

int gc_Student( lua_State *l )
{
	Student *a = (Student*)(*(void**)lua_touserdata(l,-1));
	delete a;
	return 0;
}

int l_getAge( lua_State *l )
{
	Student *a = getStudent(l);
	lua_pushinteger(l, a->getAge());
	return 1;
}

int l_setAge( lua_State *l )
{
	Student *a = getStudent(l);
	int age = luaL_checkint(l,2);
	a->setAge( age );
	return 0;
}

int l_getName( lua_State *l )
{
	Student *a = getStudent(l);
	lua_pushstring(l, a->getName().c_str());
	return 1;
}

int l_setName( lua_State *l )
{
	printf("setName\n");
	Student *a = getStudent(l);
	const char* name = luaL_checkstring(l,2);
	a->setName( name );
	return 0;
}

int l_print( lua_State *l )
{
	Student *a = getStudent(l);
	a->print();
	return 0;
}

const  luaL_reg methods[] = {
	{"print", l_print },
	{"setName", l_setName},
	{"getName", l_getName},
	{"setAge", l_setAge},
	{"getAge", l_getAge},
	{"__gc", gc_Student},
	{NULL,NULL}
};

const luaL_reg methods_f[] = {
	{ "create", newStudent },
	{ NULL, NULL}
};


extern "C" int luaopen_libstudent( lua_State *l )
{
	//1. new method table for l to save functions
	lua_newtable(l);
	int methodTable = lua_gettop(l);

	//2.new metatable for L to save "__metatable", "__index",  "__gc", etc
	luaL_newmetatable(l, CLASS_NAME );
	int metaTable = lua_gettop(l);

	//3.0 metatable["__metatable"] = methodtable;
	lua_pushliteral( l, "__metatable" );  //remove \0
	lua_pushvalue( l, methodTable );
	lua_settable( l, metaTable );

	//4.0 metatable["__index"] = methodtable
	lua_pushliteral( l, "__index" );
	lua_pushvalue( l, methodTable );
	lua_rawset( l, metaTable );  // the same as lua_settable(1,metatable)

	//5.0 metatable["__gc"] = gc_Student  //will be called when lua_close(l)
	lua_pushliteral( l, "__gc" ); 
	lua_pushcfunction( l, gc_Student );
	lua_rawset( l, metaTable );

	lua_pop(l,1);	//drop metatable

	/*6.0　for object
	name -- null set object funtion to methodtable( the table on top );
	eg: Student a = Student("xxx")
	a:func in this methodtable
	fill methodtable, is libname is not null,
	will create a table use the libname and push the table to stack*/
	luaL_openlib( l, NULL, methods, 0 ); 
	lua_pop(l,1);		//drop methodtable

	/*7.1 for class:
	name = CLASS_NAME, so this set function to "method_f"
	eg: Student a = Student:create( "xx" );
		Student:create() in this method_f tables
	*/
	luaL_openlib( l, CLASS_NAME, methods_f, 0 );  //push table[CLASS_NAME] to stack
	lua_pop(l,1); //drop table[CLASS_NAME]

	/*7.2 for class:
	add global function "CLASS_NAME", so we Student() is a global function now
	eg: Student a = Student("xx")
		function Student() in lua will call create in C++
	*/
	//lua_register(l, CLASS_NAME, create );
}

#if 0
int main(int argc, char* argv[])
{
	lua_State *l = lua_open();
	luaL_openlibs(l);
	
	Register(l);

	if( luaL_dofile( l, "teststudent.lua" )) // load and call
	{  
		printf("            ERROR          \n");
	}

	lua_close(l);

	return 0;
}
#endif