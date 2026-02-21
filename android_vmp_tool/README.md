# VMP Shield Pro - Enterprise Packer Edition

VMP Shield Pro 是一款商业级的 Android .so 加密与加壳工具。它通过将您的原始动态库加密并嵌入到一个安全的“引导外壳”中，实现在无 Root 环境下的高性能加固。

## 核心商业特性 (V11 - 最终稳定版)

1.  **全平台支持**: 完美支持 ARM (32位) 和 ARM64 (64位) 架构。
2.  **加壳技术 (Packing)**: 不仅加密代码段，而是将整个 `.so` 作为加密负载封装，极大地提高了破解难度。
3.  **无 Root 内存加载**: 使用私有的 `Memory Linker` 技术。加固后的 `.so` 在被 `System.loadLibrary` 加载时，会自动在内存中解密、映射并处理重定位。
4.  **JNI 透明切换**: 自动识别并跳转到原始 `JNI_OnLoad`，对业务代码零侵入。
5.  **可视化控制台**: 实时显示加固进度、架构检测以及内存映射状态。

## 使用指南

1.  在 AIDE 或 Android Studio 中构建并运行此项目。
2.  点击 **DEPLOY ANALYZER** 选择您想要保护的 `.so` 文件。
3.  点击 **INITIATE PROTECTION**。
4.  系统将生成一个 `packed_xxx.so` 文件在 `/sdcard/VMP/` 目录下。
5.  **集成**: 将此生成的文件放入您的 Android 工程，像往常一样加载即可。

---
由 Jules 倾力打造 - 顶级二进制安全方案。
