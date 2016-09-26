--变量定义
width=11;
height=22;
length=33;

--lua函数定义，实现加法
function sum(a, b)
    return a+b;
end

--lua函数定义，实现字符串相加
function mystrcat(a, b)
    return a .. b;
end

--lua函数定义，通过调用c代码中的cmul函数实现乘法
function luamul(a, b)
    return cmul(a, b);
end

function multireturn(a, b)
    local temp = {9, "hehehej"}
    --return temp,9,1

    return a + b, (a - b) * 2.3, a .. b, temp
end
