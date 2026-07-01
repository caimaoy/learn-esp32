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
