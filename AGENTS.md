# AGENTS.md

## 仓库结构
- 每个实验是一个独立的 PlatformIO 项目（`first-blink/`, `breathing-led/` 等）
- `wiki/` 是 LLM 维护的知识库，遵循 `LLM_WIKI.md` schema
- `LEARNING_PLAN.md` 是学习路线

## 硬件
- 开发板：ESP32 Dev Module，串口芯片 CP2102
- 设备路径：`/dev/cu.usbserial-0001`
- 板载 LED：GPIO 2，高电平点亮

## 命令
```bash
# 编译
pio run --project-dir <项目目录>

# 烧录（串口监视器必须先关闭）
pio run --target upload --project-dir <项目目录> --upload-port /dev/cu.usbserial-0001

# 串口监视器
pio device monitor -b 115200 --port /dev/cu.usbserial-0001
```

## PlatformIO 注意
- PlatformIO Core 通过 `uv tool install platformio` 安装
- 如果 esptoolpy 依赖装不上，需要先在 platformio 的 venv 里装 pip: `python -m ensurepip --upgrade`
- VS Code 的 PlatformIO 扩展有独立的 penv 环境，与 CLI 分开

## Wiki 维护（LLM 负责）
- 每次新知识记录：新建/更新 `.md` → 更新 `index.md` → 追加 `log.md`
- 日志格式：`## [YYYY-MM-DD] 操作 | 标题`
- 内容页不用注释来源，来源由日志记录
- 引用 wiki 内页面使用相对路径

## 已知坑
- 烧录前必须关掉 VS Code Serial Monitor（串口被占用会报错 `Resource temporarily unavailable`）
- `while (!Serial);` 在 ESP32 上不可靠，用 `delay(1000)` 代替
- ESP32 启动时串口输出 ROM bootloader 乱码，正常现象
