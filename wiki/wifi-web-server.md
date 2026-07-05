# WiFi AP + Web Server

## WiFi.softAP — 创建热点

```cpp
WiFi.softAP("ESP32-AP", "12345678");
```

- `WiFi.softAP()` 让 ESP32 自己当热点，不依赖外部路由器
- SSID（热点名）最长 **32 字节**
- 密码最少 **8 字节**，也可以不设密码（传空字符串）
- 默认 IP：**192.168.4.1**，DHCP 分配 192.168.4.x 给连接的设备

### 自定义 IP 网段

```cpp
WiFi.softAP("ESP32-AP", "12345678");
WiFi.softAPConfig(IPAddress(10, 0, 0, 1), IPAddress(10, 0, 0, 1), IPAddress(255, 255, 255, 0));
```

三个参数：ESP32 自身 IP、网关 IP、子网掩码。

## WebServer — HTTP 服务器

### 路由注册

```cpp
server.on("/", handleRoot);   // GET / → handleRoot()
server.on("/api", handleApi); // GET /api → handleApi()
```

`server.on()` 把 URL 路径和对应的处理函数绑定起来。支持 GET、POST 等方法。

### 入口在哪？

```cpp
server.on("/", handleRoot);  // 只注册路由
server.begin();              // 启动服务器
```

`server.begin()` 开始监听端口。之后在 `loop()` 里不断调 `handleClient()` 来检查有没有请求到。

## server.handleClient() — 核心机制

```cpp
server.handleClient();  // 放在 loop() 里
```

**特性：同步、非阻塞、轮询**

| 特性 | 说明 |
|------|------|
| **同步** | 有请求时当场执行处理函数，发完响应才返回 |
| **非阻塞** | 没请求立刻返回，不卡住主循环 |
| **轮询** | 每次调用都主动检查一次有没有新请求 |

不调 `handleClient()` → 浏览器请求永远 pending（转圈）。

### 和其他 I/O 的对比

| 模式 | 示例 | 说明 |
|------|------|------|
| 同步阻塞 | `Serial.read()` 等待数据 | 没数据就卡住 |
| 同步非阻塞 | `handleClient()` | 没请求立刻返回 |
| 异步回调 | JS `fetch().then()` | 数据到了自动触发 |

## delay(10) 的作用

```cpp
void loop() {
  sensor_read();
  server.handleClient();
  delay(10);  // 不加会怎样？
}
```

1. **防止 CPU 满速空转** — 不加的话 loop 跑几十万次/秒，白白发热
2. **给 WiFi 协议栈喘息时间** — 持续狂调 `handleClient()` 反而可能影响底层收包
3. **控制采样频率** — `delay(10)` 把 loop 限制在 ~100Hz

## JS 端轮询

```javascript
setInterval(() => {
  fetch('/api').then(r => r.json()).then(d => {
    document.getElementById('temp').textContent = d.temp + ' °C';
  });
}, 2000);  // 每 2 秒问一次
```

这不是服务器推送，是浏览器**主动轮询**（polling）。开一分钟会发 30 次请求。两个要点：

- `fetch()` 是异步的 — 不阻塞浏览器 UI
- 每次请求独立 — ESP32 没有保持连接状态

## 数据流全景

```
浏览器                     ESP32
  │                          │
  │── GET / ──────────────→  │  handleRoot() → 返回完整 HTML
  │←──── HTML 页面 ─────────│
  │                          │
  │── GET /api ───────────→  │  handleApi() → 返回 {"temp":27.5,"humi":51.2}
  │←─── JSON 数据 ──────────│
  │   (每 2 秒重复一次)       │
  │                          │
  │                 loop():  │
  │                dht.read()│→更新缓存
  │                handleClient()│→有请求就处理
  │                delay(10) │→休息
```

## 手动拼 JSON vs 库

当前代码手动拼接 JSON 字符串：

```cpp
String json = "{\"temp\":" + String(temperature, 1) +
              ",\"humi\":" + String(humidity, 1) + "}";
```

数据量小、结构简单的时候够用。复杂的 JSON 建议用 `ArduinoJson` 库。
