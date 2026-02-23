# VMP Shield Pro 部署与验证指南

本指南详细介绍了如何在 Ubuntu 系统上从零开始部署 VMP Shield Pro 混淆保护平台，并验证加密后的二进制文件运行情况。

## 1. 系统环境准备

### 推荐环境
- **操作系统**: Ubuntu 22.04 LTS 或更高版本
- **内存**: 至少 2GB
- **磁盘**: 至少 10GB 可用空间

### 安装核心依赖
执行以下命令安装编译器、交叉编译链、Redis 和 Python 环境：

```bash
sudo apt update
sudo apt install -y clang llvm redis-server python3-pip python3-venv     gcc-aarch64-linux-gnu gcc-arm-linux-gnueabi binutils-aarch64-linux-gnu
```

## 2. 项目初始化

### 克隆与依赖安装
1. 进入项目根目录。
2. 安装 Python 依赖库：

```bash
pip install -r requirements.txt
pip install lief pyelftools
```

## 3. 服务启动流程

### 第一步：启动 Redis 缓存
VMP Shield 使用 Redis 作为 Celery 的任务队列：
```bash
sudo service redis-server start
# 或者使用直接启动
redis-server --daemonize yes
```

### 第二步：启动 Celery 混淆引擎
确保当前目录在项目根目录，并设置 PYTHONPATH：
```bash
export PYTHONPATH=$PYTHONPATH:$(pwd)/backend
cd backend
celery -A celery_worker worker --loglevel=info
```

### 第三步：启动 FastAPI 后端
在另一个终端中运行：
```bash
cd backend
python3 main.py
```

### 第四步：启动前端界面
直接在浏览器中打开 `frontend/index.html`，或者使用 Python 快速启动 Web 服务器：
```bash
cd frontend
python3 -m http.server 8080
```
访问 `http://服务器IP:8080` 即可进入仪表盘。

## 4. 混淆验证测试

### 验证源码加密 (ELF)
1. 准备一个复杂的 C 代码 (`tests/complex_test.c`)。
2. 在前端上传该文件，选择目标架构（如 `x86_64`）。
3. 下载得到的 `protected_binary`。
4. 运行验证：
   ```bash
   chmod +x protected_binary
   ./protected_binary
   ```
   **预期结果**: 程序输出正常，逻辑与原始代码一致，但使用 `strings` 命令无法搜到敏感字符串。

### 验证反调试功能
尝试使用 `strace` 运行加密后的文件：
```bash
strace ./protected_binary
```
**预期结果**: 程序检测到跟踪行为并自动退出。

## 5. 常见问题 (FAQ)

- **Q: 提示 `clang: command not found`?**
  - A: 请确保执行了 `apt install clang`。
- **Q: Celery 报错 `Received unregistered task`?**
  - A: 请确保在 `backend` 目录下启动 Celery，并正确指定了 `-A celery_worker`。
- **Q: 加密后的二进制文件在 ARM 手机上无法运行？**
  - A: 请确保在上传时选择了正确的架构（AArch64），并且手机环境支持运行相应的 Linux 二进制文件。

---
*VMP Shield Pro - 为您的代码安全保驾护航*
