# Windows 10/11 顶部边缘滚轮调节亮度（无运行依赖）

这是一个纯原生 Win32 程序：鼠标移动到当前显示器上边缘时，滚动鼠标滚轮，通过 DDC/CI 调整该显示器亮度。运行时不依赖脚本或第三方软件。

## 功能
- 鼠标在显示器上边缘 `kEdgePixels` 区域时，滚轮调节亮度
- 支持多显示器（按鼠标所在显示器调节）
- 使用 DDC/CI VCP `0x10` 调整亮度

## 构建（MSVC）
需要安装 Visual Studio 或 Build Tools（仅用于编译，生成的 EXE 运行时不需要这些环境）。

在 “x64 Native Tools Command Prompt for VS” 或 “Developer PowerShell for VS” 中运行：

```powershell
cl /EHsc /O2 /DNDEBUG /DUNICODE /D_UNICODE brightness_edge.cpp ^
  /link /SUBSYSTEM:WINDOWS /OUT:brightness_edge.exe dxva2.lib user32.lib
```

构建完成后直接运行 `brightness_edge.exe`。

## 配置
编辑 `brightness_edge.cpp` 里的常量：
- `kEdgePixels`：触发区域厚度（像素）
- `kStep`：每档滚轮调整幅度（0-100 范围通常有效）
- `kPassThroughScroll`：是否允许滚轮穿透（保留原滚动行为）

## 注意
- 显示器 OSD 里需开启 DDC/CI
- 有些显示器对 VCP 0x10 的响应存在延迟或步进限制，这是硬件行为
