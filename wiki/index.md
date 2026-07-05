# ESP32 学习 Wiki 索引

## 页面列表

### 概念
- [WiFi AP + Web Server](wifi-web-server.md) — softAP、路由注册、handleClient 同步非阻塞轮询
- [ESP32 开发板概述](esp32-overview.md) — 芯片规格、开发板介绍
- [GPIO 基础](gpio-basics.md) — OUTPUT/INPUT 模式、万用表验证
- [PWM 脉冲宽度调制](pwm.md) — LEDC 通道、频率/分辨率/占空比
- [串口通信](serial-communication.md) — Serial.begin、波特率、USB 通信
- [NVS 非易失性存储](nvs.md) — Preferences 库、命名空间、分区大小

### 指南
- [开发环境搭建](setup-guide.md) — VS Code + PlatformIO + uv 安装
- [Blink 项目详解](first-blink.md) — 第一个程序逐行解释
- [DHT11 温湿度传感器](dht11.md) — 单总线通信、温湿度读取
- [WiFi 配置管理器](wifi-manager.md) — Web 页面扫描/手动输入 WiFi，NVS 持久化，自动重连
- [AP+STA 双模天气站](weather-station.md) — 双模 WiFi、NTP 时间、HTTP 天气 API、secrets.h 凭据分离
- [学习计划](../LEARNING_PLAN.md) — 学习路线图

### 常见问题
- [编译烧录问题](build-issues.md) — 缓存导致固件不工作、端口被占用
- 串口乱码 — ESP32 启动时 ROM bootloader 输出
- Serial Monitor 连接时序 — setup 打印有时看不到的问题
- [GPIO 2 与 WiFi 冲突](gpio-basics.md#已知限制gpio-2-与-wifi-冲突) — WiFi 启用后 GPIO 2 无法输出
