#!/bin/bash
# VMP Shield Pro - 宝塔面板 (BT Panel) 一键部署脚本

set -e

echo "================================================================"
echo "          VMP Shield Pro 宝塔一键部署脚本          "
echo "================================================================"

# 1. 检查并安装系统依赖
echo "[*] 正在安装系统依赖 (需要 root 权限)..."
sudo apt update
sudo apt install -y clang llvm redis-server python3-pip python3-venv     gcc-aarch64-linux-gnu gcc-arm-linux-gnueabi

# 2. 检查 Redis 状态
echo "[*] 正在启动 Redis 服务..."
if command -v systemctl >/dev/null 2>&1; then
    sudo systemctl start redis
    sudo systemctl enable redis
else
    redis-server --daemonize yes
fi

# 3. 创建虚拟环境
PROJECT_DIR=$(pwd)
echo "[*] 正在创建 Python 虚拟环境..."
if [ ! -d "venv" ]; then
    python3 -m venv venv
fi

# 4. 安装 Python 依赖
echo "[*] 正在安装 Python 核心依赖..."
./venv/bin/pip install --upgrade pip
./venv/bin/pip install -r requirements.txt
./venv/bin/pip install lief pyelftools requests

# 5. 初始化目录
echo "[*] 初始化工作目录..."
mkdir -p uploads backend/processed

# 6. 编译运行时
echo "[*] 编译运行时组件..."
clang -c runtime/runtime.c -o runtime/runtime.o

echo ""
echo "================================================================"
echo "                部署成功！请按照以下步骤完成宝塔面板配置                "
echo "================================================================"
echo ""
echo "1. 后端 (FastAPI):"
echo "   - 打开【Python项目管理器】 -> 【添加项目】"
echo "   - 项目名称: vmp_backend"
echo "   - 路径: $PROJECT_DIR"
echo "   - 启动文件: backend/main.py"
echo "   - 端口: 8000"
echo "   - 勾选【安装依赖】(或手动选择 venv 路径)"
echo ""
echo "2. 异步任务 (Celery) - 必须配置:"
echo "   - 打开【Supervisor管理器】 -> 【添加守护进程】"
echo "   - 名称: vmp_worker"
echo "   - 启动用户: root"
echo "   - 运行目录: $PROJECT_DIR"
echo "   - 启动命令: $PROJECT_DIR/venv/bin/python3 -m celery -A backend.celery_worker worker --loglevel=info"
echo ""
echo "3. 前端界面:"
echo "   - 打开【网站】 -> 【添加静态网站】"
echo "   - 根目录: $PROJECT_DIR/frontend"
echo ""
echo "4. 访问地址:"
echo "   - 访问 http://您的服务器IP:端口 即可使用。"
echo ""
echo "================================================================"
