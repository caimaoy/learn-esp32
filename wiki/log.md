# 日志

## [2026-07-01] 记录 | 开始学习 ESP32
- 确认开发板：ESP32 Dev Module (CP2102, `/dev/cu.usbserial-0001`)
- 安装环境：VS Code + PlatformIO (via uv tool install)
- 创建项目：`first-blink` (platform: espressif32, board: esp32dev, framework: arduino)
- 编译通过并成功烧录
- 学习了 Blink 代码结构 (setup/loop, pinMode, digitalWrite, Serial)
- 讨论了串口连接时序问题 (delay vs while(!Serial))
- 讨论了波特率 115200 的由来
- 讨论了 Serial.begin 不初始化会怎样
- 讨论了 OUTPUT 模式是什么、如何用万用表验证 GPIO 输出
- 讨论了上拉电阻的作用和 INPUT_PULLUP 模式
- 创建 breathing-led 项目，学习 PWM 呼吸灯
- 讨论了 PWM 通道的作用（独立发生器，互不干扰）

## [2026-07-02] 记录 | DHT11 温湿度传感器
- 新项目 `dht11-sensor`，使用 Adafruit DHT sensor library
- 接线：VCC→3.3V, DATA→GPIO4, GND→GND
- 学习了：`DHT dht(pin, type)` 对象创建、`dht.begin()`、单总线协议
- 学习了：`isnan()` 判断读取失败
- 实测：温度 27-28°C，湿度 51-54%（室温）
- 学习了 `Serial.begin` 不 `delay(1000)` 可能导致开头输出丢失
- 创建 wiki 页面 `dht11.md`
