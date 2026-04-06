# 作业1：STM32 GPIO 输入输出实验

## 作业要求

使用 STM32CubeMX 生成工程框架，在 `main.c` 中编写用户代码，通过 HAL 库实现：

- **输入**：读取按键（PB12）的电平状态
- **输出**：将读取到的电平状态直接驱动 LED（PC13）

即按键按下时 LED 亮，松开时 LED 灭（或相反，取决于硬件连接极性）。

---

## 核心代码解析

### 1. GPIO 初始化（`MX_GPIO_Init`）

```c
// 使能 GPIO 时钟
__HAL_RCC_GPIOC_CLK_ENABLE();
__HAL_RCC_GPIOB_CLK_ENABLE();

// 配置 PC13 为推挽输出
GPIO_InitStruct.Pin   = GPIO_PIN_13;
GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;  // 推挽输出
GPIO_InitStruct.Pull  = GPIO_NOPULL;          // 无上下拉
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

// 配置 PB12 为上拉输入
GPIO_InitStruct.Pin  = GPIO_PIN_12;
GPIO_InitStruct.Mode = GPIO_MODE_INPUT;       // 输入模式
GPIO_InitStruct.Pull = GPIO_PULLUP;           // 内部上拉
HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
```

**关键点：**
- 使用 GPIO 外设前必须先使能对应端口的时钟，否则寄存器写入无效。
- `GPIO_InitTypeDef` 结构体复用时，只需重新赋值需要改变的字段，再调用 `HAL_GPIO_Init`。

### 2. 主循环逻辑

```c
while (1)
{
    GPIO_PinState key_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, key_state);
}
```

- `HAL_GPIO_ReadPin` 返回 `GPIO_PIN_SET`（1）或 `GPIO_PIN_RESET`（0）。
- 将读取值直接写入输出引脚，实现输入到输出的直通映射。
- 由于 PB12 配置了内部上拉，未按键时读到高电平（SET），按下后拉低读到低电平（RESET）。

---

## 不同 GPIO 配置对结果的影响分析

### 输入配置

| 配置 | 宏定义 | 未接外部信号时的引脚状态 | 典型应用场景 |
|------|--------|--------------------------|--------------|
| 上拉输入 | `GPIO_PULLUP` | 默认高电平（1） | 按键低电平有效（按下接 GND） |
| 下拉输入 | `GPIO_PULLDOWN` | 默认低电平（0） | 按键高电平有效（按下接 VCC） |
| 浮空输入 | `GPIO_NOPULL` | 不确定，易受干扰 | 外部已有确定驱动时使用 |

**本实验使用上拉输入（`GPIO_PULLUP`）：**
- 未按键：PB12 通过内部上拉电阻接 VCC → 读到 `GPIO_PIN_SET` → PC13 输出高电平
- 按键按下（接 GND）：PB12 被拉低 → 读到 `GPIO_PIN_RESET` → PC13 输出低电平

若改为**下拉输入（`GPIO_PULLDOWN`）**，逻辑相反：
- 未按键：默认低电平 → LED 状态翻转
- 需要按键接 VCC 才能触发

若改为**浮空输入（`GPIO_NOPULL`）**：
- 引脚悬空时电平不确定，LED 会随机闪烁，不可靠

### 输出配置

| 配置 | 宏定义 | 特性 | 典型应用场景 |
|------|--------|------|--------------|
| 推挽输出 | `GPIO_MODE_OUTPUT_PP` | 可主动输出高/低电平，驱动能力强 | 直接驱动 LED、继电器等 |
| 开漏输出 | `GPIO_MODE_OUTPUT_OD` | 只能主动拉低，高电平需外部上拉 | I2C 总线、电平转换 |

**本实验使用推挽输出（`GPIO_MODE_OUTPUT_PP`）：**
- 输出高电平时，内部 P-MOS 导通，引脚直接驱动到 VCC
- 输出低电平时，内部 N-MOS 导通，引脚直接拉到 GND
- 驱动能力强，适合直接点亮 LED

若改为**开漏输出（`GPIO_MODE_OUTPUT_OD`）**：
- 写高电平时引脚处于高阻态，需外部上拉电阻才能输出高电平
- 若无外部上拉，LED 将无法被点亮（引脚悬空）

### 输出速度配置

| 配置 | 宏定义 | 最大翻转频率 | 说明 |
|------|--------|-------------|------|
| 低速 | `GPIO_SPEED_FREQ_LOW` | ~2 MHz | 功耗低，EMI 小，适合低频信号 |
| 中速 | `GPIO_SPEED_FREQ_MEDIUM` | ~10 MHz | 平衡选择 |
| 高速 | `GPIO_SPEED_FREQ_HIGH` | ~50 MHz | 适合高频通信（SPI、高速 PWM） |

本实验 LED 控制对速度无要求，使用低速即可，可减少电磁干扰。

---

## 硬件连接说明

| 引脚 | 功能 | 配置 |
|------|------|------|
| PC13 | LED 输出 | 推挽输出，低电平点亮（板载 LED 通常低有效） |
| PB12 | 按键输入 | 上拉输入，按下接 GND |
