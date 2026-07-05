# DHT11 温湿度传感器

## 引脚定义

| DHT11 | ESP32 |
|-------|-------|
| VCC   | 3.3V |
| DATA  | GPIO 4 |
| GND   | GND |

## 接线图

```
DHT11         ESP32
┌─────┐
│ VCC ├────── 3.3V
│ DATA├────── GPIO 4
│ GND ├────── GND
└─────┘
```

## 所需库

在 `platformio.ini` 中添加：

```ini
lib_deps = adafruit/DHT sensor library
```

PlatformIO 会自动下载依赖。

## 代码结构

```cpp
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(1000);          // 等串口稳定
  dht.begin();          // 初始化传感器
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("读取失败");
  } else {
    Serial.print("温度: ");
    Serial.print(t);
    Serial.println(" °C");
    Serial.print("湿度: ");
    Serial.print(h);
    Serial.println(" %");
  }

  delay(2000);
}
```

## API 说明

| 函数 | 返回值 | 说明 |
|------|--------|------|
| `dht.begin()` | void | 初始化传感器 |
| `dht.readTemperature()` | float | 读取温度（摄氏度） |
| `dht.readHumidity()` | float | 读取湿度（百分比） |
| `isnan(x)` | bool | 检查是否为"非数字"（读取失败） |

## 注意事项

- DHT11 精度：温度 ±2°C，湿度 ±5%
- 读取间隔至少 1 秒（DHT11 采样速率限制）
- 连续快速读取会返回旧值或 NaN
- `delay(1000)` 用于等待 Serial 连接稳定（ESP32 上 `while(!Serial)` 不可靠）
- 不接传感器或接线错误时 `readHumidity()` 返回 `NaN`
