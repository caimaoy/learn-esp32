# 开发环境搭建

## 环境

- **OS**: macOS (ARM64)
- **IDE**: VS Code (1.106.2)
- **Python**: 3.13.3 (via uv 0.8.11)
- **PlatformIO**: Core 6.1.19
- **框架**: Arduino (espressif32 7.0.1)

## 安装步骤

### 1. 安装 VS Code + PlatformIO 扩展
```bash
# 安装 PlatformIO IDE 扩展
code --install-extension platformio.platformio-ide
```

### 2. 安装 PlatformIO Core CLI
```bash
# 使用 uv 安装
uv tool install platformio

# 如果 esptoolpy 安装失败（pip 缺失），需要先装 pip
uv tool install pip
```

### 3. 项目结构
```
first-blink/
├── platformio.ini      # 项目配置
└── src/
    └── main.cpp        # 源代码
```

### 4. platformio.ini 配置
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
```

### 5. VS Code 操作（底部状态栏）
| 按钮 | 操作 |
|------|------|
| 🐜 PIO Home | PlatformIO 主页 |
| → (Build) | 编译 |
| →▶ (Upload) | 编译 + 烧录 |
| 🔌 | 串口监视器 |

## 常用命令
```bash
# 编译
pio run --project-dir first-blink

# 编译 + 烧录
pio run --target upload --project-dir first-blink --upload-port /dev/cu.usbserial-0001

# 串口监视器
pio device monitor -b 115200 --port /dev/cu.usbserial-0001
```
