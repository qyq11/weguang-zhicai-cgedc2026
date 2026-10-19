# 面向多通道检测的低 CPU 负载数据采集架构及其实现

**微光智采队 · 上海工程技术大学 · 2026 年第二十一届中国研究生电子设计竞赛**

本项目面向凝血分析等多通道弱光电信号检测场景，通过 STM32F407、ADS8866、ADG1208 与 TIM8 + SPI + DMA 协同，减少采样过程中的 CPU 前台阻塞，为通道扩展、数据通信和实时显示留出计算资源。

本仓库归档参赛技术报告、答辩 PPT、演示视频、展示图片和官方参赛材料。首次了解项目可从下方的[核心资料](#核心资料)开始。

## 获取仓库

先完成[环境准备](#环境准备)，再在终端执行：

```powershell
git lfs install
git clone https://github.com/qyq11/weguang-zhicai-cgedc2026.git TheChinaGraduateElectronicsDesignContest
cd TheChinaGraduateElectronicsDesignContest
git lfs pull
```

> [!IMPORTANT]
> PDF、Word、PPT、图片、视频和压缩包由 Git LFS 管理。如果文件只有包含 `version https://git-lfs.github.com/spec/v1` 的几行文本，请在仓库目录执行 `git lfs pull` 下载实际内容。

## 环境准备

| 工具 | 用途 | 版本说明 |
| --- | --- | --- |
| [Git](https://git-scm.com/downloads) | 克隆仓库和管理修订 | 本机核验版本：2.50.1（Windows） |
| [Git LFS](https://git-lfs.com/) | 下载和提交二进制资料 | 本机核验版本：3.7.0 |
| PDF 阅读器、兼容 Word/PPT 的办公软件、MP4 播放器 | 按需阅读、编辑和播放资料 | 仓库未指定版本 |

仓库未锁定 Git 或 Git LFS 版本。安装后可用以下命令检查工具是否可用：

```powershell
git --version
git lfs version
```

查看现有资料无需安装 STM32 或 Qt 开发环境。

## 核心资料

建议先看技术报告，再看答辩 PPT 和参赛视频；需要修改报告时使用 Word 原稿。

| 资料 | 入口 | 内容 |
| --- | --- | --- |
| 提交版技术报告 | [PDF](01_Documents/result/技术报告_微光智采队.pdf) · [Word](01_Documents/result/技术报告_微光智采队.docx) | 设计背景、方案论证、硬件原理、软件流程及测试结果 |
| 答辩演示文稿 | [PPTX](01_Documents/result/微光智采队_答辩PPT.pptx) | 项目介绍与答辩展示 |
| 参赛视频 | [MP4](01_Documents/result/微光智采队_参赛视频.mp4) | 项目讲解与演示 |
| 数据采集架构说明 | [PDF](02_Paper_and_Reports/面向多通道检测的低CPU负载数据采集架构.pdf) | 采集架构相关展示材料 |
| 官方赛题指南 | [PDF](01_Documents/Official_Rules/第二十一届研电赛赛题指南及清单.pdf) | 第二十一届研电赛赛题指南及清单 |
| 展示与测试素材 | [媒体目录](03_Media/) · [报告配图](02_Paper_and_Reports/picture/) | 系统架构、采样时序、测试图表及演示视频 |

## 技术方案

- **光电采集前端**：S1226 光电二极管与跨阻放大电路将微弱光电流转换为电压，ADG1208 完成通道选通，经模拟调理后由 ADS8866 进行 16 位模数转换。
- **外设协同采样**：TIM8 提供定时触发，SPI 完成串行读数，DMA 将采样结果搬运至内存；CPU 负责采集控制、状态管理和后续处理。
- **非阻塞调度与通信**：异步状态机处理通道切换与稳定等待，采样数据经 RS485 传输至 Qt 上位机，完成解析、曲线显示与存储。

![系统整体架构](03_Media/系统整体架构.png)

<details>
<summary>查看整机测试平台照片</summary>

<img src="01_Documents/result/微光智采队_展示照片4.jpg" alt="整机测试平台实拍" width="480">

</details>

## 测试结果

以下数据引自[技术报告](01_Documents/result/技术报告_微光智采队.pdf)第 5 章，三种方案的单轮扫描窗口均为 **6.98 ms**。

| 采集方案 | 前台 CPU 服务时间 | 10 s 平均 ADC 采集相关 CPU 负载率 |
| --- | --- | --- |
| 软件 CONV + SPI 轮询 | 533.62 μs | 7.62% |
| 软件 CONV + SPI DMA | 542.64 μs | 7.75% |
| TIM8 + SPI + DMA | 27.18 μs | 0.39% |

静态基准测试中，M_MIN、M_MAX 两路通道各连续采集 1000 个有效样本，标准差分别为 **0.88 LSB** 和 **1.91 LSB**。这些结果对应报告中的样机和测试条件，动态光学响应、温度漂移及长期运行表现仍需进一步验证。

## 目录结构

```text
.
├── 01_Documents/             # 参赛事务与提交材料
│   ├── Official_Rules/       # 官方说明、赛题指南及模板
│   ├── Registration/         # 报名与团队照片
│   └── result/               # 提交版报告、答辩 PPT、视频和展示照片
├── 02_Paper_and_Reports/     # 技术报告、演示文稿及相关材料
│   └── picture/              # 电路、流程、采样时序与测试结果图
├── 03_Media/                 # 展示图片、演示视频及宣传素材
│   └── result/               # 视频与展架导出文件
├── .gitattributes            # Git LFS 文件类型配置
├── .gitignore               # 构建产物与本地临时文件忽略规则
└── README.md
```

## 构建与验证范围

当前归档未包含 STM32 固件源码、Qt 上位机源码或硬件设计工程，也未提供构建、自动化测试与部署脚本。系统设计和样机实验方法见技术报告；仅凭当前仓库尚不能编译或完整复现系统。

## 资料维护与贡献

资料问题和补充建议可通过 [Issues](https://github.com/qyq11/weguang-zhicai-cgedc2026/issues) 反馈。提交 PR 时请说明资料用途、来源和主要修改，并遵循以下整理约定：

- **文件命名**：中文资料用简洁中文名，英文资料用简短英文题名；名称表达主题，按需加入年份或用途，并保留正确扩展名。
- **正式成果**：报告、展示文件和必要素材放入对应正式目录；新增图件时同时保存可编辑原稿。重命名或移动文件后，同步修正 README 和其他受影响链接。
- **临时文件**：一次性下载、辅助脚本、中间结果与日志集中放入项目根目录 `temp/`；创建该目录时附简短用途和清理说明。临时产物无需提交，正式源码和唯一原件应长期保留。
- **二进制资料**：沿用 `.gitattributes` 中的 Git LFS 配置；提交前检查资料能否正常打开、链接是否有效，以及改动是否符合用途。

## 许可证

当前仓库未附 `LICENSE` 文件。
