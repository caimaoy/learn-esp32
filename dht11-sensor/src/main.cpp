#include <Arduino.h>        // Arduino 核心库：提供 digitalWrite、delay 等所有基础 API
#include <DHT.h>            // DHT 传感器库：封装了单总线通信协议

#define DHTPIN 4            // 宏定义：DHT11 数据线接在 GPIO 4
#define DHTTYPE DHT11       // 宏定义：传感器型号是 DHT11

DHT dht(DHTPIN, DHTTYPE);   // 创建 DHT 对象，绑定引脚和型号

void setup() {              // 上电/复位后只执行一次
  Serial.begin(115200);     // 初始化 USB 串口，波特率 115200
  delay(1000);              // 等待串口连接稳定
  Serial.println("DHT11 开始");  // 串口打印启动信息
  dht.begin();              // 初始化 DHT11 传感器
}

void loop() {               // 无限循环执行
  float h = dht.readHumidity();    // 读取湿度，返回 float（浮点数）
  float t = dht.readTemperature(); // 读取温度（摄氏度），返回 float

  if (isnan(h) || isnan(t)) {      // isnan = is Not a Number，判断读取是否失败
    Serial.println("读取失败");     // 失败则打印错误
  } else {                          // 成功则打印数据
    Serial.print("温度: ");         // Serial.print 不换行
    Serial.print(t);                // 打印温度值
    Serial.print(" °C, 湿度: ");   // 打印单位
    Serial.print(h);                // 打印湿度值
    Serial.println(" %");          // println 带换行
  }

  delay(2000);             // 暂停 2 秒，再进入下一次循环
}
