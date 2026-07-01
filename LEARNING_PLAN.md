# ESP32 学习计划

## 阶段 1：搭建开发环境
- 安装 VS Code + PlatformIO 插件
- 创建新项目，选择开发板 ESP32 Dev Module
- 编译上传 Blink 示例程序

## 阶段 2：基础实验
| 实验 | 学习要点 |
|------|---------|
| LED 闪烁 | GPIO 输出、pinMode/digitalWrite |
| 按键控制 LED | GPIO 输入、digitalRead、上拉电阻 |
| PWM 调光 | analogWrite、呼吸灯 |
| 串口通信 | Serial.begin/println、打印调试 |
| DHT11/DHT22 温湿度 | 单总线协议、传感器库 |

## 阶段 3：联网
| 实验 | 学习要点 |
|------|---------|
| WiFi 连接 | WiFi.h、STA/AP 模式 |
| HTTP 请求 | 获取天气/时间 API |
| Web 服务器 | 浏览器控制 LED |

## 阶段 4：综合项目
- 远程温湿度监测站
- 智能灯控系统
