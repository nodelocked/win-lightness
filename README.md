# Windows 10/11 顶部边缘滚轮调节亮度（无运行依赖）

## 下载

在 [Release](https://github.com/nodelocked/win-lightness/releases) 页下载使用

## 使用方法
1. 运行程序
   - 双击 `brightness_edge.exe`。
   - 程序会常驻后台，无界面。
2. 调节亮度
   - 把鼠标移动到**当前显示器上边缘**，滚动鼠标滚轮即可调整亮度。
   - 不滚轮不会触发任何操作。
3. 设置开机自启动
   - 右键以 PowerShell 运行 `enable_startup.ps1`。
4. 取消开机自启动
   - 右键以 PowerShell 运行 `disable_startup.ps1`。

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

## 原理说明
- 程序注册全局低级鼠标钩子，只在**滚轮事件**发生时检测鼠标是否位于当前显示器上边缘。
- 当满足条件时，通过 DDC/CI VCP `0x10` 调整该显示器亮度。
- 多显示器场景下，按鼠标所在显示器调节，不影响其它显示器。

## 注意
- 显示器 OSD 里需开启 DDC/CI
- 有些显示器对 VCP 0x10 的响应存在延迟或步进限制，这是硬件行为
