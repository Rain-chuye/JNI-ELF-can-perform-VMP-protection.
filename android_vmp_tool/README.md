# VMP Shield for Android (AIDE Compatible)

这是一个专为 AIDE 设计的 Android 二进制保护工具项目。它能够对独立的 `.so` 文件进行指令虚拟化 (VMP)、字符串加密以及 JNI 保护。

## 核心功能

1.  **二进制 VMP (指令虚拟化)**: 自动识别 JNI 导出函数并将其关键指令转换为自定义字节码。
2.  **字符串加密**: 扫描 `.rodata` 段并对常量字符串进行 XOR 加密。
3.  **自定义 Linker (运行时存根)**: 提供轻量级的 ELF 加载器，支持在内存中动态解密并加载保护后的二进制文件。
4.  **JNI 保护**: 隐藏 `RegisterNatives` 逻辑，防止静态分析。

## 如何在 AIDE 中编译

1.  将 `android_vmp_tool` 整个文件夹导入 AIDE。
2.  确保 AIDE 已安装 NDK 支持。
3.  直接点击“运行”或“构建项目”。AIDE 会自动调用 CMake 编译 C++ 引擎。

## 使用方法

1.  运行编译出来的 APK。
2.  点击 "Select .so File" 选择一个现有的 `.so` 文件。
3.  点击 "Start Protection"。
4.  保护后的文件将保存在原目录，文件名为 `xxx_protected.so`。

## 项目结构

-   `app/src/main/cpp/engine`: 保护引擎源码 (ELF 解析、VMP 逻辑)。
-   `app/src/main/cpp/runtime`: 运行时存根 (自定义 Linker、虚拟机)。
-   `app/src/main/cpp/test`: 测试用的示例 `.so` 源码。
-   `app/src/main/java`: Android UI 与 JNI 接口。

---
由 Jules 开发。
