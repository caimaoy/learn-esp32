# WiFi 配置管理器

提供一个 Web 页面来扫描附近 WiFi、手动输入 SSID 和密码，保存到 NVS 后重启连接。连不上自动退回配置模式。

## 工作流程

```
上电 → 加载 NVS 凭据
  ├── 有凭据 → 尝试连接 STA
  │     ├── 成功 → 显示已连接页面
  │     └── 失败 → 清除凭据，进入 AP 配置模式
  └── 无凭据 → 进入 AP 配置模式
```

## 配置模式

开热点 `ESP32-Config`（密码 `12345678`），浏览器访问 `192.168.4.1`：

- **自动扫描**附近 WiFi，点击选择网络
- **手动输入** SSID（扫描不到时备用）
- 输入密码后保存并重启

```cpp
WiFi.mode(WIFI_AP_STA);        // 双模：AP + STA
WiFi.softAP(AP_SSID, AP_PASS); // 开热点
```

必须用 `WIFI_AP_STA` 而非 `WIFI_AP`，因为 WiFi 扫描依赖 STA 接口。

## NVS 凭据持久化

使用 Preferences 库将 SSID/密码存入 NVS（断电不丢）：

```cpp
prefs.begin("wifi", false);  // 读写模式，命名空间不存在时自动创建
prefs.putString("ssid", ssid);
prefs.putString("pass", pass);
```

首次启动时命名空间 `"wifi"` 不存在，如果以只读模式 (`true`) 打开会导致 `nvs_open failed: NOT_FOUND`，必须用读写模式 (`false`)。

## 扫描实现

启动扫描一次，JS 持续轮询直到完成：

```cpp
bool scanStarted = false;

String scanNetworks() {
  int n = WiFi.scanComplete();
  if (n == WIFI_SCAN_FAILED) {
    if (!scanStarted) {
      WiFi.scanNetworks(true);  // 异步扫描
      scanStarted = true;
    }
    return "{\"status\":\"scanning\"}";
  }
  if (n == WIFI_SCAN_RUNNING) {
    return "{\"status\":\"scanning\"}";
  }
  // ... 返回结果 ...
  WiFi.scanDelete();
  scanStarted = false;
}
```

- 只启动一次异步扫描，避免每次 poll 都重新开始
- 扫描不到时可通过"手动输入"兜底

## 已知坑

| 问题 | 原因 | 解决 |
|------|------|------|
| 扫描一直"进行中" | WiFi 模式设为 `WIFI_AP`，STA 接口不可用 | 改为 `WIFI_AP_STA` |
| NVS 报 NOT_FOUND | 命名空间不存在时以只读模式打开 | 用 `false`（读写模式） |
| JS 全部不执行 | C++ raw string 中 `\\'` 到 JS 里变成反斜杠+引号，导致语法错误 | 用 `\'` 而非 `\\'` |
| 串口占满上传失败 | 串口监视器占用了 `/dev/cu.usbserial-0001` | 先关闭监视器再烧录 |
