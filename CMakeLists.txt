cmake_minimum_required(VERSION 3.28)

# 初始化一个项目，并且定义项目名称和启用的语言
project(Idun LANGUAGES CXX)

# 设定C++标准版本为C++20
set(CMAKE_CXX_STANDARD 20)

# 使用Debug模式进行构建
#set(CMAKE_BUILD_TYPE Debug)

add_executable(Idun
        src/main.cpp
        src/scanner.cpp
        src/parser.cpp
        src/resolver.cpp
        src/lox_class.cpp
        src/environment.cpp
        src/interpreter.cpp
)

# 向目标（例如库或可执行文件）添加包含目录 [PUBLIC：目录对所有依赖于 cpplox 的目标可见]
target_include_directories(Idun PRIVATE inc)