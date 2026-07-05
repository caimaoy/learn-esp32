#include <Arduino.h>
#include <WiFi.h>         // ESP32 的 WiFi 库，支持 AP 和 STA 双模式
#include <WebServer.h>    // HTTP 服务器，接收浏览器请求
#include <HTTPClient.h>   // HTTP 客户端，向外部 API 发请求
#include <DHT.h>          // Adafruit DHT 传感器库
#include <time.h>         // NTP 时间同步：configTime / getLocalTime

// secrets.h 定义以下变量（被 .gitignore 排除，防密码泄露）：
//   - WIFI_SSID  你家 WiFi 名称
//   - WIFI_PASS  你家 WiFi 密码
// 参考 secrets.example.h 创建本文件
#include "secrets.h"

// AP 热点凭据（你自己设备连 ESP32 时用，不算敏感信息）
#define AP_SSID "ESP32-AP"
#define AP_PASS "12345678"

// DHT11 温湿度传感器引脚
#define DHTPIN 4
#define DHTTYPE DHT11

// Open-Meteo 天气 API 坐标（免费、无需注册、无需 API Key）
// 改成你所在位置，https://open-meteo.com 查坐标
#define LATITUDE 39.9042
#define LONGITUDE 116.4074

// HTTP 服务器和 DHT 传感器对象（全局，loop/handler 共享）
WebServer server(80);
DHT dht(DHTPIN, DHTTYPE);

// 缓存最新一次成功的传感器读数
float temperature = 0;
float humidity = 0;

// NTP 时间字符串，供页面展示
char timeStr[64] = "同步中...";

// Open-Meteo 天气缓存，每 5 分钟更新
float weatherTemp = 0;
float weatherHumidity = 0;
float weatherWindSpeed = 0;
int weatherCode = -1;
char weatherDesc[32] = "加载中...";
unsigned long lastWeatherFetch = 0;

// WMO 天气代码 → 中文描述
// 完整参考：https://open-meteo.com/en/docs#weathervariables
const char* wmoCodeToDesc(int code) {
  if (code == 0) return "晴朗";
  if (code <= 3) return "多云";
  if (code <= 48) return "雾";
  if (code <= 55) return "毛毛雨";
  if (code <= 65) return "雨";
  if (code <= 75) return "雪";
  if (code <= 99) return "雷暴";
  return "未知";
}

// 向 Open-Meteo 发起 HTTP GET 请求，手动解析 JSON 更新缓存
void fetchWeather() {
  HTTPClient http;
  http.setTimeout(10000);  // 10 秒超时，国内访问境外 API 可能慢
  String url = "http://api.open-meteo.com/v1/forecast?latitude="
             + String(LATITUDE, 4) + "&longitude=" + String(LONGITUDE, 4)
             + "&current=temperature_2m,relative_humidity_2m,weather_code,wind_speed_10m";

  http.begin(url);
  int code = http.GET();
  if (code == 200) {
    String payload = http.getString();

    // JSON 中有两个相同的 key："current_units" 段和 "current" 段各有一份
    // lastIndexOf 找到最后那个（即 "current" 段的值）
    int idx = payload.lastIndexOf("\"temperature_2m\":");
    if (idx > 0) {
      idx += 17;  // 跳过键名长度
      String val;
      while (idx < (int)payload.length() && payload[idx] != ',' && payload[idx] != '}')
        val += payload[idx++];
      weatherTemp = val.toFloat();
    }

    idx = payload.lastIndexOf("\"relative_humidity_2m\":");
    if (idx > 0) {
      idx += 22;
      String val;
      while (idx < (int)payload.length() && payload[idx] != ',' && payload[idx] != '}')
        val += payload[idx++];
      weatherHumidity = val.toFloat();
    }

    idx = payload.lastIndexOf("\"weather_code\":");
    if (idx > 0) {
      idx += 14;
      String val;
      while (idx < (int)payload.length() && payload[idx] != ',' && payload[idx] != '}')
        val += payload[idx++];
      weatherCode = val.toInt();
      strcpy(weatherDesc, wmoCodeToDesc(weatherCode));
    }

    idx = payload.lastIndexOf("\"wind_speed_10m\":");
    if (idx > 0) {
      idx += 16;
      String val;
      while (idx < (int)payload.length() && payload[idx] != ',' && payload[idx] != '}')
        val += payload[idx++];
      weatherWindSpeed = val.toFloat();
    }
  }
  http.end();
}

String buildHTML() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <title>ESP32 天气站</title>
  <style>
    body{font-family:sans-serif;text-align:center;margin:20px}
    .row{display:flex;flex-wrap:wrap;justify-content:center;gap:12px}
    .card{padding:20px;border-radius:12px;background:#f5f5f5;box-shadow:0 2px 8px rgba(0,0,0,0.1);width:260px}
    .title{font-size:18px;font-weight:bold;margin-bottom:12px}
    .row-data{margin:8px 0}
    .label{color:#666;font-size:13px}
    .value{font-size:32px;font-weight:bold}
    .value-sm{font-size:22px;font-weight:bold}
    .footer{color:#999;font-size:12px;margin-top:20px}
  </style>
</head>
<body>
  <h2>ESP32 天气站</h2>
  <div class="row">
    <div class="card">
      <div class="title">📍 室内</div>
      <div class="row-data"><div class="label">温度</div><div class="value" id="temp">--)rawliteral";
  html += String(temperature, 1);
  html += R"rawliteral(</div></div>
      <div class="row-data"><div class="label">湿度</div><div class="value" id="humi">--)rawliteral";
  html += String(humidity, 1);
  html += R"rawliteral(</div></div>
    </div>
    <div class="card">
      <div class="title">🌤 室外</div>
      <div class="row-data"><div class="label">天气</div><div class="value-sm" id="wdesc">--)rawliteral";
  html += String(weatherDesc);
  html += R"rawliteral(</div></div>
      <div class="row-data"><div class="label">温度</div><div class="value" id="wtemp">--)rawliteral";
  html += String(weatherTemp, 1);
  html += R"rawliteral(</div></div>
      <div class="row-data"><div class="label">湿度</div><div class="value" id="whumi">--)rawliteral";
  html += String(weatherHumidity, 1);
  html += R"rawliteral(</div></div>
      <div class="row-data"><div class="label">风速</div><div class="value" id="wwind">--)rawliteral";
  html += String(weatherWindSpeed, 1);
  html += R"rawliteral( km/h</div></div>
    </div>
    <div class="card">
      <div class="title">🕐 时间</div>
      <div class="value-sm" id="time">--)rawliteral";
  html += String(timeStr);
  html += R"rawliteral(</div>
    </div>
  </div>
  <div class="footer">
    室内: DHT11 | 室外: Open-Meteo | ESP32 AP+STA 双模
  </div>
  <script>
    setInterval(()=>{
      fetch('/api').then(r=>r.json()).then(d=>{
        document.getElementById('temp').textContent=d.temp;
        document.getElementById('humi').textContent=d.humi;
        document.getElementById('wdesc').textContent=d.wdesc;
        document.getElementById('wtemp').textContent=d.wtemp;
        document.getElementById('whumi').textContent=d.whumi;
        document.getElementById('wwind').textContent=d.wwind;
        document.getElementById('time').textContent=d.time;
      });
    },5000);
  </script>
</body>
</html>
)rawliteral";
  return html;
}

void handleRoot() {
  server.send(200, "text/html", buildHTML());
}

void handleApi() {
  String json = "{"
    "\"temp\":" + String(temperature, 1) + ","
    "\"humi\":" + String(humidity, 1) + ","
    "\"wtemp\":" + String(weatherTemp, 1) + ","
    "\"whumi\":" + String(weatherHumidity, 1) + ","
    "\"wwind\":" + String(weatherWindSpeed, 1) + ","
    "\"wdesc\":\"" + String(weatherDesc) + "\","
    "\"time\":\"" + String(timeStr) + "\""
    "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  delay(1000);  // 等串口连上，避免开头打印丢失

  dht.begin();

  // === 双模 WiFi：既当客户端连路由器，又当热点供直连 ===
  // STA（客户端）：连你家路由器获取互联网
  // WIFI_SSID / WIFI_PASS 定义在 secrets.h（.gitignore 排除）
  Serial.print("WiFi: ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 40) {
    delay(500);
    Serial.print(".");
    retries++;
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("STA IP: ");
    Serial.println(WiFi.localIP());  // 路由器分配的局域网 IP
  } else {
    Serial.println("STA FAILED");
  }

  // AP 模式：同时开热点，不连 WiFi 时也能访问
  WiFi.softAP(AP_SSID, AP_PASS);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());  // 固定 192.168.4.1

  // NTP 时间同步（中国 UTC+8，无需夏令时）
  configTime(8 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  // 启动时先拉一次天气
  fetchWeather();
  lastWeatherFetch = millis();

  // 注册 HTTP 路由
  server.on("/", handleRoot);    // GET / → HTML 页面
  server.on("/api", handleApi);  // GET /api → JSON 数据
  server.begin();
}

void loop() {
  // 每次 loop 都尝试读 DHT11（内部有采样周期，读太快会返回旧值）
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (!isnan(h) && !isnan(t)) {  // 读到有效数据才更新缓存
    temperature = t;
    humidity = h;
  }

  // 每秒更新一次时间字符串
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeinfo);
  }

  // 每 5 分钟拉一次天气（Open-Meteo 免费 API 有频率限制）
  if (millis() - lastWeatherFetch > 300000) {
    fetchWeather();
    lastWeatherFetch = millis();
  }

  server.handleClient();  // 非阻塞轮询：有请求就处理，没有就立刻返回
  delay(10);              // 防止 CPU 空转，给 WiFi 协议栈喘息时间
}
