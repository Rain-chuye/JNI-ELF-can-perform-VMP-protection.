# VMP Shield Pro (Stability Pass V13)

VMP Shield Pro 现已升级至 V13 版本，重点提升了加固后的 **稳定性**。解决了之前版本可能出现的“卡屏”或“无法运行”的问题。

## 核心改进 (V13)

1.  **稳定链接器 (Stable Memory Linker)**: 完整支持了 `DT_INIT_ARRAY`，确保加固后的 .so 能正确执行 C++ 静态构造函数和初始化逻辑，这是解决“卡屏”的关键。
2.  **完善重定位**: 增加了对 `R_ABS` 类型重定位的支持，适用于更复杂的动态库。
3.  **JNI 元数据同步**: 加固过程中自动提取 JNI 导出符号并在引导阶段进行校验，确保 JNI 调用链的完整性。
4.  **无 Root 内存加载**: 继续采用高性能内存加载技术，兼容所有主流 Android 系统版本。
5.  **EOF 封包技术**: 保持极小的体积开销，不会产生冗余的大文件。

## 使用说明

1.  在 AIDE/Android Studio 中编译生成 APK。
2.  运行 APK 并选择目标 `.so` 或 ELF 文件。
3.  点击 **INITIATE PROTECTION**。
4.  生成的 `packed_xxx.so` 已内置自解密与稳定加载逻辑，可直接投入生产环境。

---
由 Jules 开发 - 致力于构建 Android 平台上最稳定的二进制安全方案。
