#include <Arduino.h>
#include <WiFi.h>         // ESP32 的 WiFi 库，AP/STA 模式
#include <WebServer.h>    // HTTP 服务器，处理请求/响应
#include <DHT.h>          // Adafruit DHT 传感器库

#define DHTPIN 4
#define DHTTYPE DHT11

// WebServer 对象：监听 80 端口
WebServer server(80);

// DHT 对象：传入引脚号和传感器型号
DHT dht(DHTPIN, DHTTYPE);

// 缓存最近一次成功的读数，供 Web 页面和 API 使用
float temperature = 0;
float humidity = 0;

// 处理浏览器 GET / 请求，返回完整的 HTML 页面
void handleRoot() {
  // R"rawliteral(...)rawliteral" 是 C++ 原始字符串字面量语法
  // 括号内的内容完全按原样保留，不用转义引号或换行
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <title>ESP32 温湿度</title>
  <style>
    body{font-family:sans-serif;text-align:center;margin-top:50px}
    .card{display:inline-block;padding:30px;border-radius:12px;background:#f5f5f5;box-shadow:0 2px 8px rgba(0,0,0,0.1)}
    .sensor{margin:20px 0}
    .value{font-size:42px;font-weight:bold}
    .label{color:#666;font-size:14px}
  </style>
</head>
<body>
  <div class="card">
    <h2>ESP32 温湿度</h2>
    <div class="sensor">
      <div class="label">温度</div>
      <div class="value" id="temp">)rawliteral";
  // String(value, 1) 把 float 转成字符串，保留 1 位小数
  html += String(temperature, 1);
  html += R"rawliteral( °C</div>
    </div>
    <div class="sensor">
      <div class="label">湿度</div>
      <div class="value" id="humi">)rawliteral";
  html += String(humidity, 1);
  html += R"rawliteral( %</div>
    </div>
  </div>
  <script>
    setInterval(()=>{
      fetch('/api').then(r=>r.json()).then(d=>{
        document.getElementById('temp').textContent=d.temp+' °C';
        document.getElementById('humi').textContent=d.humi+' %';
      });
    },2000);
  </script>
</body>
</html>
)rawliteral";
  // 200 = HTTP 状态码"OK"，第二个参数告诉浏览器返回的是 HTML
  server.send(200, "text/html", html);
}

// 处理浏览器 GET /api 请求，返回 JSON 格式的温湿度数据
void handleApi() {
  // 手动拼接 JSON 字符串，没引入 ArduinoJson 库
  // String(float, 1) 控制小数位数为 1
  String json = "{\"temp\":" + String(temperature, 1) +
                ",\"humi\":" + String(humidity, 1) + "}";
  // text/html → 浏览器渲染页面，application/json → JS 用 fetch 解析
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  delay(1000);  // 等串口连上，避免开头输出丢失

  dht.begin();

  // 创建 WiFi 热点，不连外部路由器
  WiFi.softAP("ESP32-AP", "12345678");
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());  // 固定 192.168.4.1

  // 注册路由：访问 / → HTML 页面，/api → JSON 数据
  server.on("/", handleRoot);
  server.on("/api", handleApi);
  server.begin();
}

void loop() {
  // 每次 loop 都尝试读一次（DHT 内部有采样间隔，读太快会返回旧值）
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  // isnan() = "is Not-A-Number"，传感器读取出错时返回 NaN
  // 只有读到有效数据才更新缓存，否则保留上一次有效值
  if (!isnan(h) && !isnan(t)) {
    temperature = t;  // 更新缓存
    humidity = h;
  }

  // 检查有没有收到 HTTP 请求，有就调用对应的处理函数
  // 不调这行的话，浏览器请求会一直挂着不会有响应
  server.handleClient();
  delay(10);
}
