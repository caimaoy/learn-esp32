#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

// AP 热点名（ESP32 没连上 WiFi 时开这个热点供配置）
#define AP_SSID "ESP32-Config"
#define AP_PASS "12345678"

WebServer server(80);
Preferences prefs;

String savedSSID = "";
String savedPass = "";

// 从 NVS 读取已保存的 WiFi 凭据
void loadConfig() {
  prefs.begin("wifi", false);  // false = 读写模式（命名空间不存在时自动创建）
  savedSSID = prefs.getString("ssid", "");
  savedPass = prefs.getString("pass", "");
  prefs.end();
  Serial.print("Loaded config: SSID=");
  Serial.println(savedSSID == "" ? "(none)" : savedSSID.c_str());
}

// 保存 WiFi 凭据到 NVS（断电不丢）
void saveConfig(String ssid, String pass) {
  prefs.begin("wifi", false);
  prefs.putString("ssid", ssid);
  prefs.putString("pass", pass);
  prefs.end();
  Serial.println("Config saved");
}

// 清除已保存的凭据（连不上时自动调用）
void clearConfig() {
  prefs.begin("wifi", false);
  prefs.remove("ssid");
  prefs.remove("pass");
  prefs.end();
  Serial.println("Config cleared");
}

bool scanStarted = false;  // 防止每次 poll 都重新触发扫描

// 扫描附近 WiFi，返回 JSON
// 异步扫描：setup 中首次调用后，JS 持续轮询直到完成
String scanNetworks() {
  int n = WiFi.scanComplete();
  if (n == WIFI_SCAN_FAILED) {
    if (!scanStarted) {
      WiFi.scanNetworks(true);  // 异步扫描，只启动一次
      scanStarted = true;
    }
    return "{\"status\":\"scanning\"}";
  }
  if (n == WIFI_SCAN_RUNNING) {
    return "{\"status\":\"scanning\"}";
  }

  // 扫描完成，拼接结果 JSON
  String json = "{\"status\":\"done\",\"networks\":[";
  for (int i = 0; i < n; i++) {
    if (i > 0) json += ",";
    json += "{\"ssid\":\"" + WiFi.SSID(i) + "\"";
    json += ",\"rssi\":" + String(WiFi.RSSI(i));
    json += ",\"encryption\":" + String(WiFi.encryptionType(i));
    json += "}";
  }
  json += "]}";

  WiFi.scanDelete();  // 释放扫描结果
  scanStarted = false;
  return json;
}

// Web 配置页面（内联 HTML + JS）
// 注意：R"rawliteral(...)" 中 \ 是字面量，\\' 在 JS 里是反斜杠字符，不是转义引号
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <title>ESP32 WiFi 配置</title>
  <style>
    body{font-family:sans-serif;max-width:500px;margin:30px auto;padding:0 16px}
    h2{text-align:center}
    .info{background:#e8f4fd;padding:12px;border-radius:8px;margin:16px 0;font-size:14px}
    .networks{margin:16px 0}
    .network{padding:10px;border:1px solid #ddd;border-radius:8px;margin:6px 0;cursor:pointer}
    .network:hover{background:#f5f5f5}
    .network.selected{background:#d4edda;border-color:#28a745}
    .network .ssid{font-weight:bold}
    .network .rssi{float:right;color:#666;font-size:13px}
    .signal-bar{display:inline-block;width:20px;text-align:center}
    input[type=password],input[type=text]{width:100%;padding:10px;margin:6px 0;border:1px solid #ddd;border-radius:6px;box-sizing:border-box}
    button{width:100%;padding:12px;background:#007bff;color:#fff;border:none;border-radius:6px;font-size:16px;cursor:pointer}
    button:hover{background:#0056b3}
    button:disabled{background:#ccc;cursor:not-allowed}
    .error{color:#dc3545;font-size:14px;margin:8px 0}
    .rssi-dot{display:inline-block;width:8px;height:8px;border-radius:50%;margin-right:4px}
    .manual-toggle{text-align:center;margin:12px 0;font-size:14px}
    .manual-toggle span{color:#007bff;cursor:pointer;text-decoration:none}
    .manual-toggle span:hover{text-decoration:underline}
  </style>
</head>
<body>
  <h2>ESP32 WiFi 配置</h2>
  <div class="info">
     连接 ESP32-Config 热点后，从列表中选择或手动输入 WiFi 信息。
  </div>
  <div id="status" class="error" style="display:none"></div>
  <div id="networks" class="networks">
    <p>扫描中...</p>
  </div>
  <div class="manual-toggle">
    <span id="toggleLink" onclick="toggleManual()">没找到你的 WiFi？手动输入</span>
  </div>
  <div id="manualSection" style="display:none">
    <label><b>WiFi 名称 (SSID)</b></label>
    <input type="text" id="manualSsid" placeholder="输入 WiFi 名称" oninput="onManualInput()">
  </div>
  <div style="margin:12px 0">
    <label><b>密码</b></label>
    <input type="password" id="password" placeholder="输入 WiFi 密码" oninput="onManualInput()">
  </div>
  <button id="saveBtn" onclick="save()" disabled>保存并连接</button>
  <script>
    let selectedSSID = '';
    let scanCount = 0;
    let isManual = false;

    // 轮询扫描结果，最多等 15 秒（30 次 × 500ms）
    function scan() {
      fetch('/api/scan').then(r=>r.json()).then(d=>{
        if (d.status === 'scanning') {
          if (++scanCount < 30) setTimeout(scan, 500);
          return;
        }
        const container = document.getElementById('networks');
        if (d.networks.length === 0) {
          container.innerHTML = '<p>未找到 WiFi 网络。</p>';
          return;
        }
        let html = '';
        d.networks.forEach(n => {
          let bars = n.rssi > -50 ? '▂▄▆█' : n.rssi > -65 ? '▂▄▆' : n.rssi > -80 ? '▂▄' : '▂';
          let color = n.rssi > -50 ? '#28a745' : n.rssi > -65 ? '#ffc107' : '#dc3545';
          // 注意：\' 在 JS 单引号字符串中是转义的引号字符
          html += '<div class="network" onclick="select(\'' + n.ssid.replace(/'/g, "\\'") + '\', this)">';
          html += '<span class="ssid">' + n.ssid + '</span>';
          html += '<span class="rssi"><span style="color:' + color + '">' + bars + '</span> ' + n.rssi + 'dBm</span>';
          html += '</div>';
        });
        container.innerHTML = html;
      });
    }

    // 点击网络列表项
    function select(ssid, el) {
      if (isManual) toggleManual();
      document.querySelectorAll('.network').forEach(n => n.classList.remove('selected'));
      el.classList.add('selected');
      selectedSSID = ssid;
      document.getElementById('saveBtn').disabled = false;
    }

    // 切换 列表选择 / 手动输入 模式
    function toggleManual() {
      isManual = !isManual;
      const section = document.getElementById('manualSection');
      const link = document.getElementById('toggleLink');
      const networks = document.getElementById('networks');
      if (isManual) {
        section.style.display = 'block';
        link.textContent = '从列表中选择';
        networks.style.display = 'none';
        selectedSSID = '';
        document.querySelectorAll('.network').forEach(n => n.classList.remove('selected'));
      } else {
        section.style.display = 'none';
        link.textContent = '没找到你的 WiFi？手动输入';
        networks.style.display = 'block';
        document.getElementById('manualSsid').value = '';
      }
      onManualInput();
    }

    // 输入变化时更新保存按钮状态
    function onManualInput() {
      const ssid = document.getElementById('manualSsid').value.trim();
      const pass = document.getElementById('password').value;
      if (isManual) {
        document.getElementById('saveBtn').disabled = !(ssid && pass);
      } else if (!selectedSSID) {
        document.getElementById('saveBtn').disabled = true;
      }
    }

    // 提交 WiFi 凭据
    function save() {
      const pass = document.getElementById('password').value;
      const ssid = isManual ? document.getElementById('manualSsid').value.trim() : selectedSSID;
      if (!ssid) { showError('请输入或选择一个 WiFi 名称'); return; }
      if (!pass) { showError('请输入密码'); return; }
      document.getElementById('saveBtn').disabled = true;
      document.getElementById('saveBtn').textContent = '连接中...';
      const form = new URLSearchParams();
      form.append('ssid', ssid);
      form.append('pass', pass);
      fetch('/api/save', {method:'POST', body:form}).then(r=>r.text()).then(d=>{
        if (d === 'ok') {
          document.getElementById('status').style.display = 'block';
          document.getElementById('status').className = 'error';
          document.getElementById('status').textContent = '连接成功！设备即将重启...';
          document.getElementById('saveBtn').textContent = '完成';
        } else {
          showError('连接失败，请检查密码');
          document.getElementById('saveBtn').disabled = false;
          document.getElementById('saveBtn').textContent = '保存并连接';
        }
      });
    }

    function showError(msg) {
      const el = document.getElementById('status');
      el.style.display = 'block';
      el.textContent = msg;
      el.className = 'error';
    }

    scan();
  </script>
</body>
</html>
)rawliteral";
  server.send(200, "text/html", html);
}

void handleScan() {
  server.send(200, "application/json", scanNetworks());
}

void handleSave() {
  if (!server.hasArg("ssid") || !server.hasArg("pass")) {
    server.send(400, "text/plain", "missing fields");
    return;
  }
  String ssid = server.arg("ssid");
  String pass = server.arg("pass");

  saveConfig(ssid, pass);
  server.send(200, "text/plain", "ok");
  delay(500);
  ESP.restart();
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  loadConfig();

  if (savedSSID != "") {
    // 有已保存的凭据 → 尝试连接
    Serial.print("Connecting to ");
    Serial.println(savedSSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(savedSSID.c_str(), savedPass.c_str());

    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 20) {
      delay(500);
      Serial.print(".");
      retries++;
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("Connected! IP: ");
      Serial.println(WiFi.localIP());
      // 已连上，启动普通服务器（简单展示 IP）
      server.on("/", []() {
        server.send(200, "text/html",
          "<h2>ESP32 已连接</h2><p>IP: " + WiFi.localIP().toString() + "</p>"
          "<p>SSID: " + savedSSID + "</p>");
      });
      server.begin();
      return;  // 跳过 AP 模式
    }

    // 连不上 → 清除凭据，进 AP 配置模式
    Serial.println("Failed to connect, entering AP config mode");
    clearConfig();
  }

  // 没有已保存的凭据或连不上 → 开 AP 配置页
  // WIFI_AP_STA 双模：AP 开热点，STA 用于扫描 WiFi
  Serial.println("Starting AP config mode");
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(AP_SSID, AP_PASS);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/api/scan", handleScan);
  server.on("/api/save", HTTP_POST, handleSave);
  server.begin();
}

void loop() {
  server.handleClient();
  delay(10);
}
