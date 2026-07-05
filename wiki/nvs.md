# NVS 非易失性存储

NVS (Non-Volatile Storage) 是 ESP32 在 Flash 上划出的一块键值对存储区域，掉电不丢数据。适合存配置参数。

## Preferences 库

Arduino 框架通过 `Preferences` 库操作 NVS：

```cpp
Preferences prefs;

prefs.begin("namespace", false);  // 打开命名空间（false = 读写，不存在自动创建）
prefs.putString("ssid", "MyWiFi"); // 写
prefs.getString("ssid", "");      // 读（第二个参数是默认值）
prefs.remove("ssid");             // 删
prefs.end();                      // 关闭
```

- **命名空间**类似文件夹，不同功能用不同命名空间隔离（如 `"wifi"`、`"config"`）
- 支持类型：`String`、`int`、`float`、`size_t`、`blob`

## 首次打开注意事项

命名空间不存在时，以**只读模式** (`true`) 打开会报 `nvs_open failed: NOT_FOUND`。必须用**读写模式** (`false`)，命名空间会被自动创建。

```cpp
prefs.begin("wifi", true);   // 命名空间不存在 → NOT_FOUND
prefs.begin("wifi", false);  // 命名空间不存在 → 自动创建
```

## 空间大小

默认分区表下 NVS 大小为 **20KB**（`0x5000`）：

```
nvs:        offset 0x9000,  size 0x5000  = 20KB
otadata:    offset 0xE000,  size 0x2000
app0:       offset 0x10000, size 0x140000
app1:       offset 0x150000, size 0x140000
spiffs:     offset 0x290000, size 0x150000
coredump:   offset 0x3E0000, size 0x10000
```

20KB 存 WiFi 凭据绰绰有余（`putString` 每条几十字节）。

## 注意事项

- NVS 有**磨损均衡**，不会频繁写同一地址，但仍不建议高频写入
- `prefs.end()` 必须配对调用，否则可能丢数据或占锁
- 不适合存大量数据（读写慢、空间小），大数据用 SPIFFS / LittleFS
