# CatOS-Hello 测试套件

本目录包含 CatOS-Hello 的单元测试和集成测试。

## 测试结构

```
tests/
├── CMakeLists.txt              # 测试构建配置
├── README.md                   # 本文件
├── test_data/                  # 测试数据文件
│   ├── pacman.conf.sample     # 示例 pacman 配置
│   └── mirrorlist.sample      # 示例镜像列表
├── test_repolist.cpp          # RepoListWindow 测试
└── test_mirrorlist.cpp        # MirrorListWindow 测试
```

## 构建和运行测试

### 方法 1: 使用 CMake（推荐）

```bash
# 在项目根目录
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make

# 运行所有测试
ctest --output-on-failure

# 或运行特定测试
./tests/test_repolist
./tests/test_mirrorlist
```

### 方法 2: 直接在 tests 目录构建

```bash
cd tests
mkdir build && cd build
cmake ..
make
ctest --output-on-failure
```

## 测试覆盖

### RepoListWindow 测试 (`test_repolist.cpp`)

测试 pacman.conf 解析和仓库管理功能：

- ✅ `testParseKeyValue` - 键值对解析
- ✅ `testParseBasicRepo` - 基本仓库解析
- ✅ `testParseRepoWithComments` - 注释保留
- ✅ `testParseRepoWithExtraOptions` - 额外选项支持
- ✅ `testHasChanges` - 变更检测
- ✅ `testWriteConfig` - 配置写入
- ✅ `testEmptyRepo` - 空仓库处理
- ✅ `testMultipleServers` - 多服务器支持
- ✅ `testInvalidRepoNames` - 无效名称验证
- ✅ `testSigLevelOptions` - 签名级别选项
- ✅ `testCommentPreservation` - 注释保留

### MirrorListWindow 测试 (`test_mirrorlist.cpp`)

测试镜像列表获取和管理功能：

- ✅ `testParseMirrorList` - 镜像列表解析
- ✅ `testCheckRateMirrorsInstalled` - rate-mirrors 检查
- ✅ `testExtractMirrorPrefix` - URL 前缀提取
- ✅ `testValidateMirrorUrl` - URL 格式验证
- ✅ `testFilterValidMirrors` - 有效镜像过滤
- ✅ `testLimitMirrorCount` - 镜像数量限制
- ✅ `testEmptyMirrorList` - 空列表处理
- ✅ `testMirrorListWithVariousFormats` - 各种格式支持
- ✅ `testCountryNameExtraction` - 国家名称提取
- ✅ `testMirrorUrlVariables` - URL 变量替换
- ✅ `testNetworkTimeout` - 网络超时设置
- ✅ `testTemporaryFileCreation` - 临时文件创建
- ✅ `testRateMirrorsCommand` - rate-mirrors 命令参数

## 测试数据

### pacman.conf.sample

包含各种配置场景的示例 pacman 配置文件：
- options 段配置
- 标准仓库 (core, extra, multilib)
- 自定义仓库（带注释和额外选项）
- 多镜像服务器配置

### mirrorlist.sample

包含多个国家镜像的示例列表：
- 中国镜像（清华、中科大、北外等）
- 标准 Reflector 生成格式

## 已知限制

1. **集成测试限制**: 某些测试（如 `testParseBasicRepo`）需要重构代码以支持模拟文件输入，当前使用占位符。

2. **系统依赖**: `testCheckRateMirrorsInstalled` 依赖系统是否安装 `rate-mirrors`。

3. **权限测试**: 涉及 `/etc/pacman.conf` 写入的测试需要使用临时文件模拟。

## 改进建议

为了使测试更完整，建议：

1. **重构 RepoListWindow**:
   ```cpp
   // 添加构造函数参数
   RepoListWindow(const QString &configPath = "/etc/pacman.conf");
   
   // 或添加方法
   void loadConfigFromPath(const QString &path);
   ```

2. **添加 Mock 支持**: 使用 Google Mock 或 Qt 的测试工具模拟文件系统和网络操作。

3. **集成测试环境**: 设置隔离的测试环境，避免修改系统文件。

4. **性能测试**: 添加大型配置文件的解析性能测试。

5. **网络测试**: 使用 QNetworkAccessManagerMock 模拟网络请求。

## 持续集成

建议在 CI/CD 中运行测试：

```yaml
# .github/workflows/test.yml 示例
- name: Run tests
  run: |
    cd build
    ctest --output-on-failure --verbose
```

## 贡献

添加新测试时：
1. 遵循现有的命名约定 (`test*`)
2. 每个测试应该独立且可重复
3. 使用有意义的断言消息
4. 更新本 README 的测试覆盖列表
