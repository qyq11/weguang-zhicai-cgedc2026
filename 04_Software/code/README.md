# GD32H759 Audio Link EIDE Project

## 项目简介

本项目是基于兆易创新 GD32H759 系列 Cortex-M7 MCU 的 EIDE 嵌入式工程。当前代码主要用于验证音频外设链路：初始化板级基础外设，扫描 I2C 音频 Codec，配置 SAI0 与 DMA，并在主循环中运行非阻塞 LED 流水灯，便于确认系统未卡死。

工程同时保留 Keil MDK 工程文件，可通过 VS Code Embedded IDE（EIDE）或 Keil MDK 打开、编译和烧录。

## 硬件平台

- 主控芯片：GD32H759 / GD32H73x_75x 系列，Cortex-M7 内核
- 参考板卡：GD32H759I-START 评估板或兼容硬件
- 音频 ADC：ES7210，默认 7-bit I2C 地址 `0x40`
- 音频 DAC/Codec：ES8311，默认 7-bit I2C 地址 `0x18`
- 调试下载器：J-Link
- 用户 LED：GPIOC `PC9`、`PC10`、`PC11`、`PC12`

## 软件环境

- VS Code
- Embedded IDE（EIDE）插件
- Arm Compiler 6（AC6）
- GD32H73x/75x 标准外设库与 CMSIS
- Keil MDK（可选，用于打开 `project/GD32H759_project.uvprojx`）
- J-Link 驱动与工具链

EIDE 工程配置位于 `.eide/eide.yml`，当前目标为 `GD32H73x_75x`，编译器为 AC6，CPU 类型为 Cortex-M7，烧录器配置为 J-Link。

## 主要功能

- Cortex-M7 MPU、I-Cache、D-Cache 初始化
- SysTick 与 TIMER1 1 ms 时间基准
- USART0 串口调试输出
- I2C1 音频控制总线初始化与设备扫描
- ES7210、ES8311 初始化入口占位，等待补充真实寄存器表
- SAI0 双声道 16 kHz / 16 bit 音频链路配置
- DMA0 接收/发送环形缓冲区配置
- TX 缓冲区默认填充 1 kHz 正弦测试数据
- 主循环非阻塞 LED1 -> LED2 -> LED3 -> LED4 流水灯
- Keil Watch 可观察 `g_while`、`g_timer1_ms_count`、`g_while_timer_count`、`debug_i2c_addr_list`

## 目录结构说明

```text
.
├── .eide/                  # EIDE 工程配置，保留 eide.yml 等共享配置
├── App/                    # 应用入口、中断、SysTick、音频应用层
├── BSP/                    # 板级驱动：LED、I2C、SAI、DMA、USART、TIMER
├── CMSIS/                  # CMSIS 头文件与系统初始化
├── Driver/                 # ES7210、ES8311 Codec 驱动入口
├── Firmware/               # GD32H73x/75x 标准外设库源码和头文件
├── Startup/                # 启动汇编文件
├── Utilities/              # GD32H759I 评估板公共工具与 LCD/SDRAM 支持
├── project/                # Keil MDK 工程文件及 RTE 配置
├── GD32H759_project.code-workspace
└── README.md
```

已生成的 `build/`、`project/Objects/`、`project/Listings/` 等目录属于编译产物，不应提交到 GitHub。

## 启动入口与核心代码

- 启动文件：`Startup/startup_gd32h73x_75x.s`
- 系统初始化：`CMSIS/system_gd32h73x_75x.c`
- 主入口：`App/main.c`
- 中断处理：`App/gd32h73x_75x_it.c`、`BSP/bsp_audio_dma.c`
- 音频应用：`App/audio_app.c`
- 音频外设：`BSP/bsp_i2c.c`、`BSP/bsp_sai.c`、`BSP/bsp_audio_dma.c`
- Codec 驱动：`Driver/es7210.c`、`Driver/es8311.c`

## 端口配置说明

| 功能 | 外设/引脚 | 说明 |
| --- | --- | --- |
| 调试串口 | USART0：`PF4` TX，`PF5` RX | `115200-8-N-1`，用于 `printf` 输出 |
| 音频 I2C | I2C1：`PB10` SCL，`PB11` SDA | Codec 控制总线 |
| SAI MCLK | SAI0：`PE2` | 音频主时钟 |
| SAI ADC 输入 | SAI0 Block1：`PE3` SD1 | ES7210 数据输入 |
| SAI FS | SAI0：`PE4` FS0 | LRCK/帧同步 |
| SAI SCK | SAI0：`PE5` SCK0 | BCLK/位时钟 |
| SAI DAC 输出 | SAI0 Block0：`PE6` SD0 | ES8311 数据输出 |
| 用户 LED | `PC9`、`PC10`、`PC11`、`PC12` | 主循环流水灯 |
| 音频 DMA RX | DMA0 CH0，`DMA_REQUEST_SAI0_B1` | SAI 接收至 `audio_rx_buffer` |
| 音频 DMA TX | DMA0 CH1，`DMA_REQUEST_SAI0_B0` | `audio_tx_buffer` 发送至 SAI |

## 编译/烧录方法

### 使用 EIDE

1. 使用 VS Code 打开 `GD32H759_project.code-workspace` 或当前工程目录。
2. 确认已安装 Embedded IDE（EIDE）插件和 Arm Compiler 6。
3. 在 EIDE 中选择目标 `GD32H73x_75x`。
4. 执行 Build/Rebuild 生成固件。
5. 连接 J-Link 与目标板。
6. 执行 Upload/Flash 烧录到目标板。

### 使用 Keil MDK

1. 打开 `project/GD32H759_project.uvprojx`。
2. 选择对应目标并确认 AC6 工具链配置。
3. 编译工程。
4. 通过 J-Link 下载到板卡。

## 使用方法

1. 给目标板供电并连接 J-Link。
2. 连接串口工具到 USART0，波特率设置为 `115200`，数据位 8，停止位 1，无校验。
3. 烧录程序后复位板卡。
4. 串口应输出系统初始化、I2C 扫描和音频链路初始化信息。
5. 观察 `PC9` 至 `PC12` 对应 LED，应按约 200 ms 周期流水切换。
6. 若需要调试音频外设，可在 Keil Watch 中观察 DMA/I2C 相关变量。

## 注意事项

- `Driver/es7210.c` 和 `Driver/es8311.c` 当前只保留初始化入口，尚未写入真实 Codec 寄存器表。补充寄存器配置前，应先确认硬件地址、MCLK/LRCK/BCLK 比例、音频格式、通道配置和上电时序。
- 当前音频参数为 `16000 Hz`、`16 bit`、双声道。若修改采样率，需要同步核对 SAI 时钟树和 `sai_mclk_div_select()`。
- Cortex-M7 开启 D-Cache 后，DMA 缓冲区需要进行 Cache 维护。本工程已提供 RX invalidate 和 TX clean 接口。
- `.eide/eide.yml`、`project/GD32H759_project.uvprojx`、源码、启动文件和固件库是工程可编译所需文件，不要删除。
- `build/`、`project/Objects/`、`project/Listings/`、`.uvoptx`、`.uvguix.*` 等为本地生成文件或用户配置，不应提交到 GitHub。
