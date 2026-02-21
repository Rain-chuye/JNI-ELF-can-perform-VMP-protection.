# VMP Shield Ultimate (No-Root Commercial Release)

VMP Shield Ultimate 现已升级为 **无 Root 运行模式**。加固后的 .so 文件可以通过自解密技术在任何标准 Android 设备上运行。

## 核心商业特性 (V10)

1.  **无 Root 兼容性**: 采用内存加载器（Memory Linker）技术。加固后的 .so 在加载时会自动在内存中解密并重定位，无需修改系统权限。
2.  **JNI 自解密存根**: 注入了高性能的 C++ 存根，能够透明地处理加密负载。
3.  **究极指令虚拟化 (VMP)**: 针对 ARM32/64 深度优化的指令混淆。
4.  **Lua 字节码保护**: 支持对 embedded Lua 脚本的静态加密与动态还原。
5.  **商业级控制台**: 现代化的深色系专业 UI，支持实时加固日志查看。

## 使用说明

1.  在 AIDE/Android Studio 中构建并安装 APK。
2.  选择需要加固的 `.so` 或 Lua 二进制文件。
3.  点击 **INITIATE PROTECTION**。
4.  输出文件保存在 `/sdcard/VMP/`。这些文件可以直接集成到您的 Android 项目中，使用标准的 `System.loadLibrary` 即可正常运行（内部已集成自解密逻辑）。

---
*Developed by Jules - Ultimate Security Solutions.*
