require "libstudent"

print( "test lua access C++ class" )

local function main()
    --Student是个table, 调用create方法
    local s = Student:create() --lua_gettop()=2 1:table, 2:xx
    s:setAge(100)
    s:print()
    s:setName("BeiJing")
    s:setAge(1234)
    s:print()  
end

main()