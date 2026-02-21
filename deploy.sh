#!/bin/bash
# VMP Shield Pro - Ubuntu 18.04 一键部署脚本

set -e

echo "[*] 开始部署 VMP Shield Pro..."

# 1. 安装系统依赖
echo "[*] 正在安装系统依赖 (Clang, LLVM, Redis, Python3)..."
sudo apt update
sudo apt install -y clang llvm redis-server python3-pip python3-venv git nodejs npm

# 2. 启动并启用 Redis
echo "[*] 正在启动 Redis..."
sudo systemctl start redis-server
sudo systemctl enable redis-server

# 3. 创建项目目录 (假设在当前目录部署)
PROJECT_DIR=$(pwd)
echo "[*] 项目目录: $PROJECT_DIR"

# 4. 创建 Python 虚拟环境
echo "[*] 正在创建 Python 虚拟环境..."
python3 -m venv vmp_venv
source vmp_venv/bin/activate

# 5. 安装 Python 依赖
echo "[*] 正在安装 Python 依赖..."
pip install --upgrade pip
pip install -r requirements.txt

# 6. 编译运行时库
echo "[*] 正在编译 VMP 运行时..."
clang -c runtime/runtime.c -o runtime/runtime.o

# 7. 部署提示
echo ""
echo "================================================================"
echo "部署基本完成！请在宝塔面板中进行最后两步配置："
echo ""
echo "1. 后端 (FastAPI):"
echo "   - 在宝塔 Python 项目管理器中添加项目"
echo "   - 运行目录: $PROJECT_DIR"
echo "   - 启动文件: backend/main.py"
echo "   - 使用虚拟环境: $PROJECT_DIR/vmp_venv"
echo ""
echo "2. 异步队列 (Celery):"
echo "   - 在宝塔 Supervisor 管理器中添加守护进程"
echo "   - 命令: $PROJECT_DIR/vmp_venv/bin/python3 -m celery -A backend.celery_worker worker --loglevel=info"
echo "   - 运行目录: $PROJECT_DIR"
echo ""
echo "3. 前端:"
echo "   - 将宝塔网站根目录指向: $PROJECT_DIR/frontend"
echo "================================================================"
