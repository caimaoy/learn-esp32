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

## [2026-07-02] 整理 | 记录编译缓存踩坑、创建 build-issues.md
- 将 AGENTS.md 中的"已知坑"整理到 wiki
- 新建 `build-issues.md`：老旧缓存导致固件不工作 + 串口端口占用
- 更新 `index.md`

## [2026-07-05] 发现 | GPIO 2 与 WiFi 冲突
- `wifi-web-control` 中板载 LED（GPIO 2）在 WiFi AP 启动后无法输出 HIGH
- 改用 GPIO 16 可正常工作
- 记录到 `gpio-basics.md` 已知限制章节
- 简化代码：去掉 LED 控制，只保留 DHT11 温湿度显示（HTTP + JSON API）
- 串口输出乱码疑似因 WebServer 占用 CPU，但网页正常

## [2026-07-05] 新增 | WiFi AP + Web Server wiki 页面
- 新建 `wifi-web-server.md`，覆盖：
  - `WiFi.softAP()` 的 SSID/密码规则和默认 IP
  - `server.on()` 路由注册和 `begin()` 启动
  - `handleClient()` 同步非阻塞轮询机制
  - `delay(10)` 控制 CPU 占用和 WiFi 稳定性
  - JS 轮询 vs 服务器推送的区别
  - 数据流全景图

## [2026-07-05] 新项目 | apsta-weather — AP+STA 双模天气站
- 新建 `apsta-weather` 项目，集成三块数据：
  - 室内：DHT11 温湿度（本地传感器）
  - 室外：Open-Meteo 免费天气 API（温度、湿度、风速、天气描述）
  - 时间：NTP 网络时间同步
- 双模 WiFi：STA 连用户路由器 + AP 开热点直连
- 引入 `secrets.h` 凭据分离模式（.gitignore 排除，参考 `secrets.example.h`）
- 手动解析 JSON：发现 `indexOf` 找到 `current_units` 段（非目标），改用 `lastIndexOf` 定位 `current` 段
- 创建 `weather-station.md` wiki 页面，记录：双模配置、NTP、HTTP 客户端、JSON 解析、secrets.h 模式
