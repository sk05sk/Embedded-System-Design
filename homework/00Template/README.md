# 嵌入式系统设计作业报告模板说明

本目录提供一份统一的中文 LaTeX 实验报告模板，适用于本课程基于 STM32F103C8T6 的作业报告撰写。模板文件为 `embedded_homework_report.tex`。

## 模板特点

1. 使用 `ctexart`，适合 XeLaTeX 编译中文文档。
2. 已预置封面、目录、实验环境、CubeMX 配置、程序设计、流程图、实验结果、问题分析、总结、附录等章节。
3. 已预置图片插入示例，便于加入硬件连接图、CubeMX 截图、实验结果图、串口助手截图等内容。
4. 已预置程序流程图示例，学生可直接修改节点文字后复用。

## 建议使用方式

1. 复制 `embedded_homework_report.tex` 到自己的作业目录，或直接在本目录内修改。
2. 将实验截图放入 `figures/` 目录。
3. 使用 XeLaTeX 编译生成 PDF。

## 建议图片命名

1. `figures/cubemx-config.png`：CubeMX 关键配置截图。
2. `figures/code-main.png`：关键代码截图。
3. `figures/result-photo.jpg`：实验现象照片。
4. `figures/result-serial.png`：串口调试助手或终端结果截图。

## 编译命令示例

```bash
xelatex embedded_homework_report.tex
xelatex embedded_homework_report.tex
```

如果本机安装了 `latexmk`，也可以使用：

```bash
latexmk -xelatex embedded_homework_report.tex
```

## 写作建议

1. 不要只放代码，要解释配置原因和现象。
2. 不要只放截图，要说明截图证明了什么。
3. 结果分析要对应本次作业的验收目标。
4. 若实验未成功，也要提交真实的排查过程和改进思路。