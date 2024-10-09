# M5Cardputer Wi-Fi 扫描器

这个项目是为M5Cardputer设备开发的一个Wi-Fi扫描器程序，可以显示周围的Wi-Fi网络及其信号强度，并提供图形化的信号强度分析。

## 功能

- 显示启动界面，包含 "WiFi Analyzer" ASCII art 和版本号，持续 0.5 秒。
- 扫描并显示周围的Wi-Fi网络。
- 显示Wi-Fi网络名称和信号强度（以dB为单位）。
- 使用左右箭头键切换显示不同的Wi-Fi网络。
- 每30秒自动重新扫描Wi-Fi网络。
- 在屏幕周围显示绿色边框。
- 在屏幕下半部分显示类似信号分析仪的图形，展示所有搜索到的WiFi信号强度曲线。
- 使用丰富的彩虹色谱和互补色来区分不同的WiFi信号曲线，确保相邻频道的颜色差异明显。
- 当前选中的WiFi信号曲线使用明亮的颜色高亮显示，其他信号使用暗色显示。
- 在屏幕上方显示左右翻页指示器，当达到列表边界时变为灰色。
- 高亮显示当前选中的WiFi信号曲线。
- 在图形底部显示WiFi信道编号，曲线的峰值对应于各自的信道位置。

## 使用方法

1. 确保您已经安装了Arduino IDE和必要的M5Cardputer库。
2. 打开`demo.ino`文件。
3. 将代码上传到您的M5Cardputer设备。
4. 上传完成后，设备将首先显示 0.5 秒的启动界面，然后自动开始扫描Wi-Fi网络。
5. 使用左箭头键（','）显示上一个Wi-Fi网络，使用右箭头键（'/'）显示下一个Wi-Fi网络。屏幕上方的三角形指示器会显示是否可以继续翻页。
6. 观察屏幕下半部分的信号强度曲线图，了解各个WiFi网络的信号分布情况。

## 代码说明

- `showSplashScreen()` 函数显示启动界面，包含 ASCII art 和版本号。
- `setup()` 函数初始化设备，显示启动界面，并执行首次Wi-Fi扫描。
- `loop()` 函数处理键盘输入和定期重新扫描。
- `scanWiFi()` 函数执行Wi-Fi扫描并存储结果。
- `displayWiFi()` 函数在屏幕上显示当前选中的Wi-Fi网络信息和绿色边框。
- `drawSignalGraph()` 函数使用贝塞尔曲线绘制平滑的信号强度曲线图。
- `drawPageIndicators()` 函数绘制左右翻页指示三角形。
- `getComplementaryColor()`, `getBrightColor()`, 和 `getDarkColor()` 函数用于生成和调整颜色。

## 注意事项

- 确保M5Cardputer设备电量充足。
- Wi-Fi扫描可能会消耗较多电量，请注意设备的使用时间。
- 在某些地区，频繁的Wi-Fi扫描可能受到法律限制，请遵守当地法规。

## 未来改进

- 优化信号曲线的显示，以适应更多的WiFi网络和信道。
- 添加保存感兴趣的Wi-Fi网络功能。
- 实现连接到选定Wi-Fi网络的功能。
- 优化电源管理，延长电池使用时间。
- 实现自适应颜色选择算法，以更好地处理大量WiFi网络的情况。
- 进一步优化颜色选择算法，以适应更多的WiFi网络。

如果您有任何问题或建议，请随时提出。祝您使用愉快！