#!/bin/bash

# CatOS-Hello 测试运行脚本

set -e

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}   CatOS-Hello 测试套件${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""

# 检查构建目录
if [ ! -d "build" ]; then
    echo -e "${YELLOW}创建构建目录...${NC}"
    mkdir build
fi

cd build

# 配置 CMake
echo -e "${YELLOW}配置 CMake（启用测试）...${NC}"
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON

# 编译（自动检测系统获取 CPU 核心数）
echo -e "${YELLOW}编译项目...${NC}"
if command -v nproc &> /dev/null; then
    make -j$(nproc)
else
    # macOS 没有 nproc，使用 sysctl
    make -j$(sysctl -n hw.ncpu)
fi

# 运行测试
echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}   运行测试${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""

if ctest --output-on-failure --verbose; then
    echo ""
    echo -e "${GREEN}========================================${NC}"
    echo -e "${GREEN}   所有测试通过! ✓${NC}"
    echo -e "${GREEN}========================================${NC}"
    exit 0
else
    echo ""
    echo -e "${RED}========================================${NC}"
    echo -e "${RED}   测试失败! ✗${NC}"
    echo -e "${RED}========================================${NC}"
    exit 1
fi
