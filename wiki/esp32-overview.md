# ESP32 开发板概述

## 硬件信息（当前开发板）

- **芯片**: ESP32-D0WD-V3 (rev v3.1)
- **特性**: WiFi, BT, 双核 240MHz
- **Flash**: 4MB
- **串口芯片**: Silicon Labs CP2102
- **串口设备**: `/dev/cu.usbserial-0001`
- **板载 LED**: GPIO 2（蓝色 LED，低电平熄灭 / 高电平点亮）

## ESP32 特点

| 特性 | 说明 |
|------|------|
| 双核处理器 | Xtensa LX6 @ 240MHz |
| WiFi | 802.11 b/g/n |
| 蓝牙 | BLE 4.2 + Classic BT |
| GPIO | 34 个可编程引脚 |
| ADC | 2 个 12-bit SAR ADC |
| 功耗 | 深度睡眠 ~5μA |

## 常见开发板型号

- **ESP32 Dev Module** — 标准开发板，用的最多
- **NodeMCU-32S** — 类似，通常带 CP2102 或 CH340
- **ESP32-S3** — 新一代，USB OTG + AI 加速
