require "libgenerateCppSo"

for k, v in pairs(libgenerateCppSo)
    do 
        print(k, v)
    end
--	cpp_cat function: 0x11fcd90
--	cpp_mul function: 0x11fcd30
--	cpp_print_int_string_float      function: 0x11fcdf0


print(libgenerateCppSo.cpp_cat(2, 32))  -- 2_32
print(libgenerateCppSo.cpp_mul(2, 3))   -- 6
print(libgenerateCppSo.cpp_mul(2, 6))   -- 12
print(libgenerateCppSo.cpp_cat(2, 456)) -- 2_456

print(libgenerateCppSo.cpp_cat("2", "rty456$%")) -- 2_rty456$%
	
print(libgenerateCppSo.cpp_print_int_string_float(5, "hello_world", 3.4567))
-- cpp_print_int_string_float : 5 hello_world 3.456700	

-- print(libgenerateCppSo.cpp_cat(2, 45b6))   -- stdin:1: malformed number near '45b6'
--print(libgenerateCppSo.cpp_cat(2, "45b6")) -- 2_45b6