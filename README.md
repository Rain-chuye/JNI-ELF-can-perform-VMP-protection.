# VMP Shield Pro - 高级二进制与 JNI 保护系统

VMP Shield Pro 是一个专为 Android 和 Linux 设计的商业级二进制保护平台。它结合了 LLVM 指令虚拟化 (VMP)、控制流扁平化 (CFG Flattening) 和高级静态二进制加密技术。

## 核心功能

- **控制流扁平化 (CFG Flattening)**: 将程序的原始逻辑转换为基于调度器 (Dispatcher) 的扁平化结构，彻底破坏自动化逆向分析工具的控制流图。
- **高级字符串加密**: 采用随机密钥对每个常量字符串进行独立加密，并在加载时自动解密。
- **自定义 VMP 虚拟机**: 支持算术运算、逻辑判断的虚拟化执行，保护敏感算法逻辑。
- **ELF 节加密 (Section Encryption)**: 对 .so 文件的关键节（如 .rodata）进行加密，防止静态分析。
- **增强型反调试 (Anti-Debug)**: 集成基于 `ptrace` 和 `TracerPid` 检查的主动加固功能，防止 IDA/GDB 挂载。
- **多架构支持**: 完美支持 x86_64, ARM, AArch64 (Android 64-bit)。

## 快速开始

### 1. 部署环境
详细部署步骤请参考 [DEPLOY_GUIDE.md](DEPLOY_GUIDE.md)。

### 2. 启动服务
```bash
# 启动 Redis
redis-server --daemonize yes

# 启动后端
cd backend
python3 main.py &
export PYTHONPATH=$PYTHONPATH:$(pwd)
celery -A celery_worker worker --loglevel=info &
```

### 3. 访问界面
在浏览器中打开 `frontend/index.html`，即可开始保护您的二进制文件。

## 技术路线 (Current Progress)
- [x] 控制流扁平化实现
- [x] 动态随机密钥字符串加密
- [x] 跨架构 (ARM/ARM64) 编译支持
- [x] 增强型反调试与加固
- [x] 现代化安全仪表盘与评估系统

---
*本系统致力于为您的核心资产提供坚不可摧的代码盾牌。*
