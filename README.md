# VMP Shield Pro - 二进制与 JNI 保护系统 (MVP)

VMP Shield Pro 是一个专为 Android 和 Linux 设计的商业级二进制保护平台。它结合了 LLVM 指令虚拟化 (VMP) 和静态二进制加密技术，能够有效防止代码被逆向工程、反编译或非法篡改。

## 核心功能

- **指令虚拟化 (VMP)**: 将关键的 C/C++ 指令转换为自定义字节码，并在嵌入式虚拟机中执行，彻底破坏反汇编器的逻辑分析。
- **字符串加密 (String Obfuscation)**: 自动识别并加密二进制文件中的常量字符串（如 API 密钥、URL），防止通过 `strings` 命令进行静态泄露。
- **JNI 自动化保护**: 支持 Android NDK 编译后的 `.so` 文件加密，保护原生库的安全性。
- **自动化处理**: 提供 Web 界面，用户只需上传源码或二进制文件即可完成加密。
- **高性能运行时**: 轻量级的 C 运行时库，在保证安全的同时最大限度降低性能损耗。

## 项目架构

- **前端 (Frontend)**: 基于 React + Tailwind CSS 的现代化仪表盘。
- **后端 (Backend)**: 使用 FastAPI + Celery + Redis 构建的异步任务处理系统。
- **引擎 (Engine)**:
  - 基于 LLVM IR 的源代码级混淆引擎。
  - 基于 ELF 结构的二进制级保护引擎。
- **运行时 (Runtime)**: 提供 C 语言实现的虚拟机解释器和解密模块。

---

## 环境配置与搭建教程

### 1. 系统要求
- **操作系统**: Ubuntu 20.04+ 或其他 Linux 发行版。
- **编译器**: LLVM/Clang 15.0+。
- **后端**: Python 3.10+。
- **中间件**: Redis Server。

### 2. 安装依赖项

#### 系统工具安装
```bash
sudo apt update
sudo apt install -y clang llvm redis-server python3-pip nodejs npm
```

#### Python 环境配置
```bash
pip install fastapi uvicorn celery redis python-multipart
```

### 3. 项目启动步骤

#### 第一步：启动 Redis 服务
```bash
sudo service redis-server start
```

#### 第二步：启动 Celery 异步任务队列
进入项目根目录执行：
```bash
export PYTHONPATH=$PYTHONPATH:$(pwd)
celery -A backend.celery_worker worker --loglevel=info
```

#### 第三步：启动 FastAPI 后端服务
```bash
python3 backend/main.py
```

#### 第四步：访问前端界面
直接在浏览器中打开 `frontend/index.html`。或者使用简单的 Web 服务器：
```bash
cd frontend && python3 -m http.server 8080
```
然后访问 `http://localhost:8080`。

---

## 使用指南

1. **源码保护**:
   - 上传 `.c` 或 `.cpp` 文件。
   - 在关键函数上添加标记：`__attribute__((annotate("vmp")))`。
   - 系统将自动进行指令虚拟化和字符串加密。

2. **二进制保护**:
   - 直接上传编译好的 `.so` 文件。
   - 系统将应用二进制级别的静态加密，隐藏关键常量信息。

## 技术路线 (Roadmap)
- [ ] 支持更多 LLVM 混淆模式（虚假控制流、指令替换）。
- [ ] 实现更复杂的 ELF 段重构，增强对 `.so` 文件的二进制级混淆。
- [ ] 增加多用户计费与权限管理模块。
- [ ] 集成 Android 模拟器自动运行测试。

---
*本系统由 Jules 开发，旨在为商业软件提供坚实的代码盾牌。*

---

## 宝塔面板 (BT Panel) 安装教程

如果您希望在宝塔面板上部署此系统，请参考以下步骤：

### 1. 环境准备
- 在宝塔面板“软件商店”中安装：
  - **Redis** (必需)
  - **Python项目管理器** (建议 2.0+)
  - **Supervisor管理器** (用于守护 Celery 进程)

### 2. 上传代码
- 将整个项目文件夹上传到服务器（例如 `/www/wwwroot/vmp_shield`）。

### 3. 配置 Python 项目 (FastAPI)
- 打开 **Python项目管理器**，点击“添加项目”：
  - **项目名称**: `vmp_backend`
  - **路径**: 选择 `/www/wwwroot/vmp_shield`
  - **Python版本**: 选择 3.10+
  - **启动文件**: `backend/main.py`
  - **端口**: `8000`
  - **勾选**: “安装依赖” (系统会自动读取 `requirements.txt`)
- **重点：** 项目添加成功后，在项目列表中点击“路径”，找到该项目的虚拟环境路径。通常是 `/www/wwwroot/vmp_shield/vmp_backend_venv/bin/python`（具体名称请在目录中确认）。

### 4. 配置 Celery 守护进程 (Supervisor)
如果您遇到 `No module named celery`，是因为 Supervisor 使用了系统 Python 而不是项目的虚拟环境 Python。

- 打开 **Supervisor管理器**，点击“添加守护进程”：
  - **名称**: `vmp_worker`
  - **启动用户**: `www` 或 `root`
  - **运行目录**: `/www/wwwroot/vmp_shield`
  - **启动命令**: `[虚拟环境Python路径] -m celery -A backend.celery_worker worker --loglevel=info`
    - *示例：* `/www/wwwroot/vmp_shield/vmp_backend_venv/bin/python3 -m celery -A backend.celery_worker worker --loglevel=info`
  - **进程数量**: 1
- 保存并启动。

### 5. 部署前端
- 在宝塔面板“网站”中添加一个“静态网站”。
- 根目录指向 `/www/wwwroot/vmp_shield/frontend`。

### 6. 常见问题排查 (FAQ)
- **Q: 启动 Celery 提示没有名为 celery 的模块？**
  - **A:** 请确保在 Supervisor 的“启动命令”中使用的是**项目虚拟环境**下的 Python 路径。不要直接写 `python3`，要写类似 `/www/wwwroot/vmp_shield/xxx_venv/bin/python3` 的全路径。
- **Q: 编译器找不到 clang？**
  - **A:** 在宝塔终端执行 `sudo apt install -y clang`。
- **Q: 任务一直处于 processing 状态？**
  - **A:** 请检查 Supervisor 中的 `vmp_worker` 是否正常运行。如果已启动但无效，查看 Supervisor 日志确认是否有权限或路径错误。

---

---

## Ubuntu 18.04 一键部署 (推荐)

如果您使用的是 Ubuntu 18.04 干净系统，可以直接在终端执行：

```bash
# 下载代码后进入项目目录
chmod +x deploy.sh
./deploy.sh
```

该脚本会自动处理大部分系统依赖和环境初始化工作。

---

---

## Android 应用 (APK)

我们为您生成了一个 Android 端的 WebApp (VMP Shield)。
- **下载地址**: 项目根目录下的 `VMP_Shield_v1.0.apk`。
- **功能**: 自动连接到您的保护服务器，方便在手机上管理加密任务。

---
