# GPIO 基础

## 什么是 GPIO？

GPIO = General Purpose Input/Output（通用输入输出引脚）。ESP32 的每个引脚可以设成输入或输出模式。

## OUTPUT 模式

**比喻：电源插座**

| 操作 | 效果 |
|------|------|
| `digitalWrite(pin, HIGH)` | 引脚输出 **3.3V** |
| `digitalWrite(pin, LOW)` | 引脚输出 **0V** |

用来控制：LED、蜂鸣器、马达、继电器等需要电压驱动的外设。

**调用顺序：**
```cpp
pinMode(GPIO2, OUTPUT);      // 先设成输出模式
digitalWrite(GPIO2, HIGH);   // 再输出高电平
```

## INPUT 模式

**比喻：门铃按钮**

用来检测外部信号：按键按没按、传感器输出高还是低。

```cpp
pinMode(GPIO4, INPUT);          // 设成输入模式
int val = digitalRead(GPIO4);   // 读取电平：HIGH 或 LOW
```

## INPUT_PULLUP — 内部上拉电阻

按键接法中，为了让引脚不按时读到稳定的 HIGH，需要接一个上拉电阻。

**原理：**
- 不按时：电阻把引脚拉到 3.3V → 读到 `HIGH`
- 按下时：引脚直接连 GND（电阻被短路）→ 读到 `LOW`

**不用外部电阻的做法（推荐）：**
```cpp
pinMode(GPIO4, INPUT_PULLUP);  // 启用 ESP32 内部上拉电阻
```
ESP32 内部有可编程上拉电阻，省掉外部 10kΩ 电阻，少接一根线。

**不上拉会怎样？**
引脚悬空（floating），电压不稳定，会感应环境电磁干扰，读数随机跳变。

## 如何验证 OUTPUT 在输出？

1. **万用表**：红笔接 GPIO 引脚，黑笔接 GND
   - 输出 HIGH → 测到 **3.3V**
   - 输出 LOW → 测到 **0V**
2. **逻辑分析仪**：可以看到电压变化波形
3. **示波器**：专业工具，能看到电压随时间变化

## 开发板上的引脚标识

ESP32 Dev Module 排针上标有：
- `D2` / `IO2` / `GPIO2` — GPIO 2
- `D4` / `IO4` / `GPIO4` — GPIO 4
- `GND` — 地线
- `3V3` — 3.3V 电源输出
- `5V` — 5V 电源输出

## 已知限制：GPIO 2 与 WiFi 冲突

**现象：** 启用 `WiFi.softAP()` 后，`digitalWrite(GPIO2, HIGH)` 无法点亮板载 LED。
同一份代码去掉 WiFi 即可正常工作。

**推测原因：** GPIO 2 是 ESP32 的 strapping 引脚（MTDI），WiFi 协议栈启动后可能修改了引脚配置。

**解决方案：**
- 使用其他空闲引脚（如 GPIO 16、GPIO 17）驱动 LED
- 如果必须用 GPIO 2，确认 `pinMode` 在 `WiFi.softAP()` **之后**调用（但实测仍然无效）
