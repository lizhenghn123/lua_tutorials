--lua函数定义，通过调用c代码中的cmul函数实现乘法和减法
function luamul_sub(a, b)
    return cmul_sub(a, b);
end

--lua函数定义，通过调用c代码中的ccat函数实现字符串连接
function luacat(a, b, c)
    c = c .. "__modifyByLua"  -- 模拟在lua中先对输入进行处理
    return ccat(a, b, c);
end