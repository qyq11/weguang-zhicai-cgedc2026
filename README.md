# 微光智采队 · 2026 研电赛项目归档

本仓库保存微光智采队参加第二十一届中国研究生电子设计竞赛的项目资料，参赛作品为 **《面向多通道检测的低 CPU 负载数据采集架构及其实现》**，参赛单位为上海工程技术大学。

项目面向多通道弱光电信号检测，以 STM32F407、ADS8866 和 ADG1208 为核心，通过 TIM8、SPI 与 DMA 协同完成采样触发、串行读数和数据搬运，并结合非阻塞采集状态机、RS485 通信与 Qt 上位机实现数据采集和显示。

## 获取仓库

仓库使用 Git LFS 管理 PDF、Word、PPT、图片、视频和压缩包等二进制资料。克隆前请安装 [Git](https://git-scm.com/downloads) 和 [Git LFS](https://git-lfs.com/)，并确认当前账号具有仓库访问权限。

```powershell
git --version
git lfs version
git lfs install
git clone https://github.com/qyq11/weguang-zhicai-cgedc2026.git TheChinaGraduateElectronicsDesignContest
cd TheChinaGraduateElectronicsDesignContest
git lfs pull
```

若下载到的文档或视频只是包含 `version https://git-lfs.github.com/spec/v1` 的几行文本，说明拿到的是 LFS 指针文件；请在仓库目录执行 `git lfs pull` 下载实际内容。

## 阅读与使用

本仓库以参赛材料归档为主。阅读 PDF 需要 PDF 阅读器；编辑 DOC/DOCX 或 PPTX 需要兼容的办公软件；观看 MP4 需要视频播放器。

建议按以下顺序浏览：

1. [技术报告（PDF）](02_Paper_and_Reports/技术报告_微光智采队.pdf)：了解项目背景、硬件原理、软件流程与测试结果。
2. [答辩 PPT](01_Documents/result/微光智采队_答辩PPT.pptx)：快速查看项目展示内容。
3. [参赛视频](01_Documents/result/微光智采队_参赛视频.mp4)：查看项目讲解和演示。
4. [系统整体架构图](03_Media/系统整体架构.png)与[实物图](03_Media/实物图.png)：查看系统组成和样机。

## 目录说明

```text
.
├── 01_Documents/                 # 官方材料、报名资料与提交成果
│   ├── Official_Rules/           # 参赛说明、赛题指南及模板
│   ├── Registration/             # 报名与团队照片
│   └── result/                   # 提交版报告、答辩 PPT、视频和展示照片
├── 02_Paper_and_Reports/         # 技术报告、演示文稿及论文配图
│   └── picture/                  # 电路、流程与测试结果等图件
├── 03_Media/                     # 展示图片、演示视频及宣传素材
│   └── result/                   # 视频与展架导出文件
├── .gitattributes               # Git LFS 文件类型配置
├── .gitignore                   # 构建产物与本地临时文件忽略规则
└── README.md
```

其中 `01_Documents/result/` 保存提交材料，`02_Paper_and_Reports/` 保存报告、演示文稿和配图，`03_Media/` 保存制作展示内容时使用的媒体素材。

## 技术方案与测试结果

- **采集前端**：S1226 光电二极管与跨阻放大电路完成光电转换，ADG1208 完成多通道选通，ADS8866 完成 16 位模数转换。
- **采样与传输**：STM32F407 通过 TIM8 + SPI + DMA 协同降低采样过程中的 CPU 参与；异步状态机处理通道切换与稳定等待，数据经 RS485 传至 Qt 上位机。
- **验证方法**：比较不同采集方案的 CPU 服务时间，并通过 M_MIN、M_MAX 静态基准通道评价采集链路稳定性。

以下数据引自仓库内技术报告，测试条件为当前样机、6.98 ms 单轮扫描窗口：

| 指标 | 报告记录结果 |
| --- | --- |
| TIM8 + SPI + DMA 前台 CPU 服务时间 | 27.18 μs |
| 该方案 10 s 平均 ADC 采集相关 CPU 负载率 | 0.39% |
| M_MIN 通道标准差（1000 点） | 0.88 LSB |
| M_MAX 通道标准差（1000 点） | 1.91 LSB |

具体测试设置、对照方案和适用范围见[技术报告](02_Paper_and_Reports/技术报告_微光智采队.pdf)第 5 章。

## 开发与验证范围

当前仓库未包含 STM32 固件源码、Qt 上位机源码或硬件设计工程，因此没有可直接执行的软件安装、开发服务器、编译、自动化测试或部署流程。技术报告描述了系统设计和样机测试，可作为理解方案与后续补充工程文件的参考。

## 资料维护

补充资料时请按用途放入对应目录；重命名或移动文件后同步更新 README 链接。图片、视频和文档继续沿用 `.gitattributes` 中的 Git LFS 配置。

当前仓库未附 `LICENSE` 文件。
