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


<!-- headroom:rtk-instructions -->
# RTK (Rust Token Killer) - Token-Optimized Commands

When running shell commands, **always prefix with `rtk`**. This reduces context
usage by 60-90% with zero behavior change. If rtk has no filter for a command,
it passes through unchanged — so it is always safe to use.

## Key Commands
```bash
# Git (59-80% savings)
rtk git status          rtk git diff            rtk git log

# Files & Search (60-75% savings)
rtk ls <path>           rtk read <file>         rtk grep <pattern>
rtk find <pattern>      rtk diff <file>

# Test (90-99% savings) — shows failures only
rtk pytest tests/       rtk cargo test          rtk test <cmd>

# Build & Lint (80-90% savings) — shows errors only
rtk tsc                 rtk lint                rtk cargo build
rtk prettier --check    rtk mypy                rtk ruff check

# Analysis (70-90% savings)
rtk err <cmd>           rtk log <file>          rtk json <file>
rtk summary <cmd>       rtk deps                rtk env

# GitHub (26-87% savings)
rtk gh pr view <n>      rtk gh run list         rtk gh issue list

# Infrastructure (85% savings)
rtk docker ps           rtk kubectl get         rtk docker logs <c>

# Package managers (70-90% savings)
rtk pip list            rtk pnpm install        rtk npm run <script>
```

## Rules
- In command chains, prefix each segment: `rtk git add . && rtk git commit -m "msg"`
- For debugging, use raw command without rtk prefix
- `rtk proxy <cmd>` runs command without filtering but tracks usage
<!-- /headroom:rtk-instructions -->
