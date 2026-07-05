# AP+STA 双模天气站

结合之前学到的知识，做一个综合项目：ESP32 同时当 AP 热点 + 连路由器上网，展现 DHT11 室内温度、NTP 时间和 Open-Meteo 室外天气。

## AP+STA 双模

```cpp
WiFi.mode(WIFI_AP_STA);                      // 设为双模
WiFi.begin(WIFI_SSID, WIFI_PASS);            // STA：连路由器
WiFi.softAP("ESP32-AP", "12345678");          // AP：开热点
```

- `WIFI_AP_STA` 模式同时启用客户端和热点
- STA 连接路由器获取互联网（用于 NTP / 天气 API）
- AP 让手机直连 ESP32，不依赖路由器也能访问网页

## 凭据分离 — secrets.h

为防止 WiFi 密码意外提交到 Git，将敏感信息放到单独文件并忽略：

**`secrets.h`**（已加入 `.gitignore`，不会提交）：
```cpp
#define WIFI_SSID "你家WiFi名字"
#define WIFI_PASS "你家WiFi密码"
```

**`secrets.example.h`**（可提交的模板）：
```cpp
#define WIFI_SSID "your_wifi_name"
#define WIFI_PASS "your_wifi_password"
```

**`main.cpp`** 中引入：
```cpp
// secrets.h 定义 WIFI_SSID / WIFI_PASS（被 .gitignore 排除）
#include "secrets.h"
```

这种做法叫 **credentials separation**，其他敏感配置（API Key、Token）同理。

## NTP 时间同步

ESP32 内置 NTP 客户端，无需额外库：

```cpp
#include <time.h>

// 在 setup 中配置（中国 UTC+8）
configTime(8 * 3600, 0, "pool.ntp.org", "time.nist.gov");

// 在 loop 中读取
struct tm timeinfo;
if (getLocalTime(&timeinfo)) {
  strftime(timeStr, 64, "%Y-%m-%d %H:%M:%S", &timeinfo);
}
```

- `configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2)`
- 中国时区 `gmtOffset_sec = 8 * 3600`，无夏令时
- `strftime` 按指定格式转成字符串

## HTTP 客户端 —— 拉取天气数据

```cpp
#include <HTTPClient.h>

HTTPClient http;
http.setTimeout(10000);           // 超时 10 秒
http.begin("http://api.open-meteo.com/v1/forecast?latitude=...&longitude=...");
int code = http.GET();            // 发起 GET 请求
if (code == 200) {
  String payload = http.getString();  // 获取响应体
}
http.end();                       // 释放资源
```

### Open-Meteo API

- 免费、无需注册、无 API Key
- 端点：`https://api.open-meteo.com/v1/forecast`
- 参数：`latitude`、`longitude`、`current= temperature_2m,relative_humidity_2m,weather_code,wind_speed_10m`
- 查询坐标：https://open-meteo.com

### 手动 JSON 解析（轻量场景）

返回的 JSON 有两层结构：
```json
{
  "current_units": {"temperature_2m": "°C", ...},
  "current": {"temperature_2m": 28.0, ...}
}
```

两个段有**相同的 key 名**。用 `lastIndexOf` 找到最后一个（即 `current` 段）：

```cpp
int idx = payload.lastIndexOf("\"temperature_2m\":");
if (idx > 0) {
  idx += 17;  // 跳过键名 "temperature_2m":
  String val;
  while (payload[idx] != ',' && payload[idx] != '}')
    val += payload[idx++];
  weatherTemp = val.toFloat();
}
```

数据量小、结构简单时手动解析够用。复杂 JSON 推荐 `ArduinoJson` 库。

## 数据缓存

```cpp
unsigned long lastWeatherFetch = 0;

void loop() {
  // 每 5 分钟拉一次
  if (millis() - lastWeatherFetch > 300000) {
    fetchWeather();
    lastWeatherFetch = millis();
  }
}
```

- `millis()` 返回开机以来的毫秒数（约 50 天后溢出归零）
- 缓存最新数据，网页请求时不阻塞等待传感器/API
- 天气不需要实时更新，5 分钟够用
