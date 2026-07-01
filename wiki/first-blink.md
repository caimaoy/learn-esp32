# Blink 项目详解

## 完整代码（含注释）

```cpp
#include <Arduino.h>              // 引入 Arduino 核心库，提供所有 API

#define LED_BUILTIN 2             // 宏定义：LED 接在 GPIO 2（ESP32 板载 LED）

void setup() {                    // 上电/复位后只执行一次
  pinMode(LED_BUILTIN, OUTPUT);   // 设置 GPIO 2 为输出模式
  Serial.begin(115200);           // 初始化 USB 串口通信，波特率 115200
  delay(1000);                    // 给串口监视器留时间连接
  Serial.println("ESP32 Blink 开始!"); // 通过 USB 向电脑发送字符串
}

void loop() {                     // 无限循环执行
  digitalWrite(LED_BUILTIN, HIGH); // 输出高电平 (3.3V) → LED 亮
  Serial.println("LED ON");       // 串口打印 "LED ON"
  delay(2000);                    // 暂停 2000 毫秒
  digitalWrite(LED_BUILTIN, LOW);  // 输出低电平 (0V) → LED 灭
  Serial.println("LED OFF");      // 串口打印 "LED OFF"
  delay(2000);                    // 暂停 2000 毫秒
}
```

## 关键函数

| 函数 | 作用 |
|------|------|
| `pinMode(pin, mode)` | 设置引脚为 OUTPUT 或 INPUT（详见 [GPIO 基础](gpio-basics.md)） |
| `digitalWrite(pin, value)` | 输出 HIGH (3.3V) 或 LOW (0V) |
| `digitalRead(pin)` | 读取引脚电平 |
| `Serial.begin(baud)` | 初始化串口通信 |
| `Serial.println(msg)` | 发送文本到串口 |
| `delay(ms)` | 暂停指定毫秒数（阻塞） |

## 核心概念

### setup vs loop
- **`setup()`**：上电后执行一次，用于初始化
- **`loop()`**：setup 执行完后无限循环，主要逻辑写在这里

### 为什么 GPIO 2？
板载 LED 硬件设计时焊接在 GPIO 2 引脚，所以 `LED_BUILTIN` 定义为 2。不同开发板可能不同（如 GPIO 5、GPIO 16）。

### 串口输出
- `Serial.println()` 将消息发往电脑（USB）
- VS Code 串口监视器 (🔌) 可以看到输出
- 波特率必须与 platformio.ini 的 `monitor_speed` 一致
- ESP32 启动时会有 ROM bootloader 输出乱码，属于正常现象
