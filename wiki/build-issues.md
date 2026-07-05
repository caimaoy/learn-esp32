# 编译烧录常见问题

## 老旧编译缓存导致固件不工作

### 现象
- 固件编译成功，上传成功，但板载 LED 只在启动时闪一下就再也不动
- `firmware.bin` 体积异常小（~240KB vs 正常 ~269KB）

### 原因
PlatformIO 的编译缓存会失效：代码改了但部分 .o 文件没重新编译，导致链接出的二进制是旧的。

### 解决方法
```bash
pio run --target clean --project-dir <项目目录>
pio run --target upload --project-dir <项目目录> --upload-port /dev/cu.usbserial-0001
```

每次烧录前都做一次 clean 可以避免。

## serial monitor 端口被占用

### 现象
```
A fatal error occurred: Could not open /dev/cu.usbserial-0001, the port is busy or doesn't exist.
```

### 原因
VS Code Serial Monitor 或之前打开的 terminal 还在占用串口。

### 解决
先关 VS Code Serial Monitor，或用以下命令释放：
```bash
lsof /dev/cu.usbserial-0001 | awk 'NR>1{print $2}' | xargs kill -9
```
