cmake_minimum_required(VERSION 4.0.0)

project(CatOS-Hello-Tests)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找 Qt 测试模块
find_package(Qt6 REQUIRED COMPONENTS Test Widgets Network)

# 启用测试
enable_testing()

# 设置父目录路径
set(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

# 包含父目录的头文件
include_directories(${PARENT_DIR})

# 定义公共源文件（不包含 main.cpp）
set(COMMON_SOURCES
    ${PARENT_DIR}/RepoListWindow.cpp
    ${PARENT_DIR}/RepoListWindow.h
    ${PARENT_DIR}/MirrorListWindow.cpp
    ${PARENT_DIR}/MirrorListWindow.h
)

# RepoListWindow 测试
add_executable(test_repolist
    test_repolist.cpp
    ${PARENT_DIR}/RepoListWindow.cpp
    ${PARENT_DIR}/RepoListWindow.h
)

target_link_libraries(test_repolist
    Qt6::Test
    Qt6::Widgets
    Qt6::Network
)

add_test(NAME test_repolist COMMAND test_repolist)

# MirrorListWindow 测试
add_executable(test_mirrorlist
    test_mirrorlist.cpp
    ${PARENT_DIR}/MirrorListWindow.cpp
    ${PARENT_DIR}/MirrorListWindow.h
)

target_link_libraries(test_mirrorlist
    Qt6::Test
    Qt6::Widgets
    Qt6::Network
)

add_test(NAME test_mirrorlist COMMAND test_mirrorlist)

# 设置测试输出目录
set_target_properties(test_repolist test_mirrorlist
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)

# 复制测试数据
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_data
     DESTINATION ${CMAKE_BINARY_DIR}/tests)

message(STATUS "测试已配置，运行 'ctest' 或 'make test' 来执行测试")
