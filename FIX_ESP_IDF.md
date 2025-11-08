# 修复 ESP-IDF 路径问题

## 问题

ESP-IDF 扩展配置指向 `/Volumes/Michael/Coding/cardputer/esp-idf`，但该目录不存在。

## 快速解决方案

### 方案 1: 使用 ESP-IDF 扩展重新安装（最简单）

1. **打开命令面板** (`Cmd+Shift+P`)
2. **运行命令**: `ESP-IDF: Configure ESP-IDF extension`
3. **选择**: "Express" 安装方式
4. **选择版本**: v5.4 或 v6.0（推荐 v5.4，因为项目文档提到 v5.4）
5. **选择安装路径**: 使用默认路径或选择 `/Volumes/Michael/Coding/cardputer/esp-idf`
6. **等待安装完成**

### 方案 2: 手动安装并创建符号链接

如果扩展安装失败，可以手动安装：

```bash
# 1. 创建目录
mkdir -p /Volumes/Michael/Coding/cardputer
cd /Volumes/Michael/Coding/cardputer

# 2. 克隆 ESP-IDF（使用 v5.4.1，项目推荐版本）
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
git checkout v5.4.1
git submodule update --init --recursive

# 3. 安装工具（只需要 ESP32-S3 支持）
./install.sh esp32s3

# 4. 验证安装
. ./export.sh
idf.py --version
```

### 方案 3: 更新配置文件指向现有安装

如果你在其他位置已经安装了 ESP-IDF：

```bash
# 假设 ESP-IDF 安装在 ~/esp/esp-idf
# 创建符号链接
mkdir -p /Volumes/Michael/Coding/cardputer
ln -s ~/esp/esp-idf /Volumes/Michael/Coding/cardputer/esp-idf
```

然后更新配置文件 `~/.espressif/esp_idf.json`：

```json
{
  "idfInstalled": {
    "esp-idf-9a814224d663de7ecfc9939dabb8e3f8": {
      "version": "5.4.1",
      "python": "/Users/michael/.espressif/python_env/idf5.4_py3.13_env/bin/python",
      "path": "/Volumes/Michael/Coding/cardputer/esp-idf"
    }
  }
}
```

## 验证修复

修复后，在 Cursor/VSCode 中：

1. **重新加载窗口**: `Cmd+Shift+P` -> `Developer: Reload Window`
2. **打开终端**，运行：
   ```bash
   get_idf
   idf.py --version
   ```
3. **应该能看到 ESP-IDF 版本信息**

## 推荐操作

**建议使用方案 1**（扩展自动安装），因为：
- 最简单，一键完成
- 自动配置所有路径
- 自动安装所需工具
- 与 Cursor/VSCode 扩展完美集成

安装完成后，错误应该就会消失。

