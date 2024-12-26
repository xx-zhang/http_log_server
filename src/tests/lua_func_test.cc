extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <iostream>
#include <string>

void callLuaClassMethod(const std::string& filename) {
    lua_State* L = luaL_newstate(); // 创建新的 Lua 状态
    luaL_openlibs(L); // 打开 Lua 标准库

    // 加载并执行 Lua 文件
    if (luaL_dofile(L, filename.c_str()) != LUA_OK) {
        std::cerr << "Lua error: " << lua_tostring(L, -1) << std::endl;
        lua_close(L);
        return;
    }

    // 获取 Lua 中的实例
    lua_getglobal(L, "instance");

    // 获取 greet 方法
    lua_getfield(L, -1, "greet");

    // 将实例作为第一个参数传递给方法
    lua_pushvalue(L, -2);

    // 传递方法参数
    lua_pushstring(L, "Hi");

    // 调用方法（1 个参数，1 个返回值）
    if (lua_pcall(L, 2, 1, 0) != LUA_OK) {
        std::cerr << "Lua error: " << lua_tostring(L, -1) << std::endl;
        lua_close(L);
        return;
    }

    // 获取返回值
    if (lua_isstring(L, -1)) {
        std::string result = lua_tostring(L, -1);
        std::cout << "Lua method returned: " << result << std::endl;
    }

    lua_pop(L, 1); // 移除返回值
    lua_close(L); // 关闭 Lua 状态
}

int main() {
    std::string luaFile = "/root/a.lua"; // 你的 Lua 文件名
    callLuaClassMethod(luaFile);

    return 0;
}
//  g++ -llua ./lua_func_test.cc 

/**
-- 定义一个类
MyClass = {}
MyClass.__index = MyClass

-- 构造函数
function MyClass:new(name, age)
    local obj = setmetatable({}, MyClass)
    obj.name = name
    obj.age = age
    return obj
end

-- 类方法
function MyClass:greet(greeting)
    return greeting .. ", my name is " .. self.name .. " and I am " .. self.age .. " years old."
end

-- 创建一个实例并调用方法
instance = MyClass:new("Alice", 28)
result = instance:greet("Hello")
 */
