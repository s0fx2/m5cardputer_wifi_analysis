/**
 * @file display.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief M5Cardputer Display Test
 * @version 0.1
 * @date 2023-10-09
 *
 *
 * @Hardwares: M5Cardputer
 * @Platform Version: Arduino M5Stack Board Manager v2.0.7
 * @Dependent Library:
 * M5GFX: https://github.com/m5stack/M5GFX
 * M5Unified: https://github.com/m5stack/M5Unified
 */
#include <M5Cardputer.h>
#include <WiFi.h>
#include <vector>
#include <algorithm>

struct WifiInfo {
    String ssid;
    int32_t rssi;
    int32_t channel;
    uint32_t color;
};

std::vector<WifiInfo> wifiNetworks;
int currentWifiIndex = 0;
String currentSSID = "";

const int CHANNEL_DISPLAY_COUNT = 9;
const int GRAPH_HEIGHT = 100;
const int CHANNEL_WIDTH = 20;

// 扩展的彩虹色谱（使用RGB565格式）
const uint16_t rainbowColors[] = {
    0xF800, // 红
    0xFD20, // 橙
    0xFFE0, // 黄
    0x07E0, // 绿
    0x001F, // 蓝
    0x781F, // 靛
    0xF81F, // 紫
    0xFC9F, // 粉
    0x07FF, // 青
    0x83FF, // 天蓝
    0x801F, // 深紫
    0xFB56, // 玫瑰
    0xFE19  // 珊瑚
};
const int numRainbowColors = sizeof(rainbowColors) / sizeof(rainbowColors[0]);

uint32_t getComplementaryColor(uint32_t color) {
    uint8_t r = (color >> 11) & 0x1F;
    uint8_t g = (color >> 5) & 0x3F;
    uint8_t b = color & 0x1F;
    return ((31-r) << 11) | ((63-g) << 5) | (31-b);
}

uint16_t getBrightColor(uint16_t color) {
    uint8_t r = min(31, ((color >> 11) & 0x1F) + 5);
    uint8_t g = min(63, ((color >> 5) & 0x3F) + 10);
    uint8_t b = min(31, (color & 0x1F) + 5);
    return (r << 11) | (g << 5) | b;
}

uint16_t getDarkColor(uint16_t color) {
    uint8_t r = max(0, ((color >> 11) & 0x1F) - 5);
    uint8_t g = max(0, ((color >> 5) & 0x3F) - 10);
    uint8_t b = max(0, (color & 0x1F) - 5);
    return (r << 11) | (g << 5) | b;
}

const char* VERSION = "v1.0.0";

void setup() {
    auto cfg = M5.config();
    M5Cardputer.begin(cfg);
    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.clear();
    
    showSplashScreen();
    
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    scanWiFi();
    displayWiFi();  // 直接显示扫描结果
}

void loop() {
    M5Cardputer.update();

    if (M5Cardputer.Keyboard.isKeyPressed('/')) {  // 右箭头键
        if (currentWifiIndex < wifiNetworks.size() - 1) {
            currentWifiIndex++;
            currentSSID = wifiNetworks[currentWifiIndex].ssid;
            displayWiFi();
        }
        delay(200);  // 防抖
    }
    if (M5Cardputer.Keyboard.isKeyPressed(',')) {  // 左箭头键
        if (currentWifiIndex > 0) {
            currentWifiIndex--;
            currentSSID = wifiNetworks[currentWifiIndex].ssid;
            displayWiFi();
        }
        delay(200);  // 防抖
    }

    static unsigned long lastScanTime = 0;
    if (millis() - lastScanTime > 30000) {
        scanWiFi();
        lastScanTime = millis();
        displayWiFi();  // 确保在重新扫描后更新显示
    }
}

void scanWiFi() {
    int n = WiFi.scanNetworks();
    std::vector<WifiInfo> newWifiNetworks;
    
    for (int i = 0; i < n; ++i) {
        WifiInfo info;
        info.ssid = WiFi.SSID(i);
        info.rssi = WiFi.RSSI(i);
        info.channel = WiFi.channel(i);
        info.color = rainbowColors[i % numRainbowColors];
        newWifiNetworks.push_back(info);
    }
    
    std::sort(newWifiNetworks.begin(), newWifiNetworks.end(), 
              [](const WifiInfo& a, const WifiInfo& b) { return a.channel < b.channel; });
    
    // 保存当前选中的SSID
    String previousSSID = currentSSID;
    
    // 查找当前选中的SSID在新扫描结果中的索引
    int newIndex = -1;
    for (size_t i = 0; i < newWifiNetworks.size(); ++i) {
        if (newWifiNetworks[i].ssid == previousSSID) {
            newIndex = i;
            break;
        }
    }
    
    // 更新全局变量
    wifiNetworks = std::move(newWifiNetworks);
    
    // 如果找到了之前选中的SSID，保持选中状态
    if (newIndex != -1) {
        currentWifiIndex = newIndex;
        currentSSID = previousSSID;
    } else {
        // 如果之前选中的SSID不在新的扫描结果中，保持当前索引不变（除非超出范围）
        if (currentWifiIndex >= wifiNetworks.size()) {
            currentWifiIndex = wifiNetworks.size() - 1;
        }
        if (!wifiNetworks.empty()) {
            currentSSID = wifiNetworks[currentWifiIndex].ssid;
        } else {
            currentWifiIndex = 0;
            currentSSID = "";
        }
    }
}

void displayWiFi() {
    M5Cardputer.Display.clear();
    
    // 绘制绿色边框
    M5Cardputer.Display.drawRect(0, 0, M5Cardputer.Display.width(), M5Cardputer.Display.height(), GREEN);
    
    if (!wifiNetworks.empty()) {
        // 显示当前选中的WiFi名称和信号强度
        String currentWifi = wifiNetworks[currentWifiIndex].ssid + " (" + String(wifiNetworks[currentWifiIndex].rssi) + " dB)";
        
        // 恢复原来的字体大小
        M5Cardputer.Display.setTextSize(1);  // 将字体大小改回 1
        M5Cardputer.Display.setTextColor(WHITE);
        
        int16_t textWidth = M5Cardputer.Display.textWidth(currentWifi);
        int16_t x = (M5Cardputer.Display.width() - textWidth) / 2;
        M5Cardputer.Display.setCursor(x, 10);  // 稍微调整文本位置
        M5Cardputer.Display.print(currentWifi);
        
        // 绘制左右翻页指示三角形
        drawPageIndicators();
        
        // 绘制分隔线
        M5Cardputer.Display.drawLine(0, 30, M5Cardputer.Display.width(), 30, GREEN);
        
        // 绘制信号强度曲线
        drawSignalGraph();
    } else {
        M5Cardputer.Display.setTextSize(1);  // 这里也改回原来的大小
        M5Cardputer.Display.setCursor(5, 10);
        M5Cardputer.Display.print("No networks found");
    }
}

void drawPageIndicators() {
    // 左三角
    uint32_t leftColor = (currentWifiIndex > 0) ? YELLOW : DARKGREY;
    M5Cardputer.Display.fillTriangle(5, 15, 15, 10, 15, 20, leftColor);
    
    // 右三角
    uint32_t rightColor = (currentWifiIndex < wifiNetworks.size() - 1) ? YELLOW : DARKGREY;
    int rightX = M5Cardputer.Display.width() - 15;
    M5Cardputer.Display.fillTriangle(rightX, 10, rightX + 10, 15, rightX, 20, rightColor);
}

void drawSignalGraph() {
    int graphBottom = M5Cardputer.Display.height() - 20;
    int graphTop = 35;  // 保持图表顶部位置不变
    int graphHeight = graphBottom - graphTop;
    int centerX = M5Cardputer.Display.width() / 2;
    int centerChannel = wifiNetworks[currentWifiIndex].channel;
    
    // 绘制信道号
    for (int i = 0; i < CHANNEL_DISPLAY_COUNT; i++) {
        int channel = centerChannel - 4 + i;
        if (channel > 0 && channel <= 14) {
            int x = centerX + (i - 4) * CHANNEL_WIDTH;
            M5Cardputer.Display.setCursor(x, graphBottom + 5);
            M5Cardputer.Display.print(channel);
        }
    }
    
    // 绘制信号强度曲线
    for (size_t i = 0; i < wifiNetworks.size(); i++) {
        const auto& wifi = wifiNetworks[i];
        int x = centerX + (wifi.channel - centerChannel) * CHANNEL_WIDTH;
        int signalHeight = map(wifi.rssi, -100, -30, 0, graphHeight * 1.4);  // 增加40%的高度
        signalHeight = constrain(signalHeight, 0, graphHeight);  // 确保高度不超过图表范围
        uint16_t color;
        if (i == currentWifiIndex) {
            color = WHITE;  // 当前选中的曲线使用亮白色
        } else {
            color = getDarkColor(wifi.color);  // 其他曲线使用暗色
        }
        
        // 绘制贝塞尔曲线
        int x1 = x - CHANNEL_WIDTH;
        int y1 = graphBottom;
        int x2 = x;
        int y2 = graphTop + graphHeight - signalHeight;  // 从顶部开始画
        int x3 = x + CHANNEL_WIDTH;
        int y3 = graphBottom;
        
        for (float t = 0; t <= 1; t += 0.005) {  // 保持精度以获得平滑的曲线
            float xt = pow(1-t, 2) * x1 + 2 * (1-t) * t * x2 + pow(t, 2) * x3;
            float yt = pow(1-t, 2) * y1 + 2 * (1-t) * t * y2 + pow(t, 2) * y3;
            M5Cardputer.Display.drawPixel(xt, yt, color);
        }
    }
}

void showSplashScreen() {
    M5Cardputer.Display.clear();
    M5Cardputer.Display.setTextColor(WHITE);
    M5Cardputer.Display.setTextSize(1);
    
    // ASCII Art for "WiFi Analyzer"
    const char* asciiArt[] = {
        "  _       ___  _____ _    ",
        " | |     / (_)/ ____(_)   ",
        " | | /| / / _| |    _     ",
        " | |/ |/ / | | |   | |    ",
        " |__/|__/|_| |_|   |_|    ",
        "    _                _    ",
        "   / \\   _ __   __ _| |_   _ _______ _ __ ",
        "  / _ \\ | '_ \\ / _` | | | | |_  / _ \\ '__|",
        " / ___ \\| | | | (_| | | |_| |/ /  __/ |   ",
        "/_/   \\_\\_| |_|\\__,_|_|\\__, /___\\___|_|   ",
        "                       |___/              "
    };

    int startY = (M5Cardputer.Display.height() - 11 * 8) / 2;  // Center vertically

    for (int i = 0; i < 11; i++) {
        int textWidth = M5Cardputer.Display.textWidth(asciiArt[i]);
        int startX = (M5Cardputer.Display.width() - textWidth) / 2;
        M5Cardputer.Display.setCursor(startX, startY + i * 8);
        M5Cardputer.Display.print(asciiArt[i]);
    }

    // Display version
    M5Cardputer.Display.setCursor((M5Cardputer.Display.width() - M5Cardputer.Display.textWidth(VERSION)) / 2, 
                                  startY + 11 * 8 + 8);
    M5Cardputer.Display.print(VERSION);

    M5Cardputer.Display.display();
    delay(500);  // 显示 0.5 秒
}