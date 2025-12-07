# STM32 IoT 智能温湿度与门控系统 (STM32 Smart Environment & Door Control)

[![Microcontroller](https://img.shields.io/badge/Microcontroller-STM32F103C8T6-blue.svg)](https://www.st.com/en/microcontrollers-microprocessors/stm32f103c8.html)
[![Language](https://img.shields.io/badge/Language-C-orange.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Status](https://img.shields.io/badge/Status-Developing-green.svg)](#)
## 项目简介 (Introduction)

本项目基于 **STM32F103C8T6** 单片机开发，集成环境监测与远程控制功能。系统能够实时采集温湿度数据，通过 OLED 屏幕本地显示，并利用 ESP8266 Wi-Fi 模块与服务器/APP 通信，实现数据的远程上传以及对舵机（门锁）的远程控制。

适用于智能家居、环境监控或物联网入门学习项目。

## 功能特性 (Features)

* **环境监测**：利用 **DHT11** 传感器实时采集环境温度和湿度数据。
* **本地显示**：通过 **0.96寸 OLED** 屏幕实时刷新显示当前状态、温湿度数值及联网情况。
* **远程通信**：集成 **ESP8266** 模块，支持连接 Wi-Fi，通过 MQTT/TCP 协议将数据上传至云端/APP。
* **门控执行**：使用 **PWM** 驱动舵机（如 SG90），实现门锁的开关控制。
* **状态指示**：板载 **LED** 指示系统运行状态或网络连接状态。
* **用户交互**：预留 **按键 (Key)** 接口，支持本地手动触发控制。

## 硬件清单 (Hardware Requirements)

| 硬件名称 | 型号/描述 | 备注 |
| :--- | :--- | :--- |
| 主控芯片 | STM32F103C8T6 | 核心控制器 |
| 温湿度传感器 | DHT11 | 单总线通信 |
| 显示屏 | 0.96" OLED | I2C / SPI 接口 (具体见代码) |
| Wi-Fi 模块 | ESP8266 (ESP-01S) | 串口通信 (AT指令) |
| 舵机 | SG90 / MG995 | PWM 控制门锁 |
| 调试/下载器 | ST-Link V2 或 J-Link | 用于程序下载 |
| 其他 | 按键, LED, 杜邦线, 面包板/PCB | 基础外围 |

## 引脚配置 (Pin Configuration)

> **注意**：请务必根据 `Hardware/` 目录下的头文件确认实际引脚连接，以下为通用参考：

| 模块 (Module) | STM32 引脚 (Pin) | 功能描述 | 对应文件 |
| :--- | :--- | :--- | :--- |
| **DHT11** | `PAx` / `PBx` | 数据引脚 (Data) | `Hardware/dht11.h` |
| **OLED** | `PB6` (SCL), `PB7` (SDA) | I2C 接口 (示例) | `Hardware/oled.h` |
| **ESP8266** | `PA2` (TX), `PA3` (RX) | USART2 串口通信 | `ESP/esp8266.h` |
| **舵机 (PWM)** | `PA1` / `PA6` | TIMx PWM 输出 | `Hardware/pwm.h` |
| **LED** | `PC13` / `PA1` | 状态指示灯 | `Hardware/led.h` |
| **按键 (Key)** | `PA0` / `PB12` | 输入控制 | `Hardware/key.h` |

## 软件环境 (Software Setup)

1.  **IDE**: Keil uVision 5 (MDK-ARM)
2.  **库函数**: STM32F10x Standard Peripheral Library (标准库)
3.  **编译器**: ARMCC (AC5/AC6)
4.  **调试工具**: 串口调试助手 (XCOM/Vofa+)

## 使用说明 (Quick Start)

1.  **硬件连接**：按照引脚配置表连接好所有硬件模块。
2.  **修改配置**：
    * 打开 `ESP/esp8266.c` 或 `main.c`，修改 Wi-Fi 名称 (SSID) 和密码 (Password)。
    * 修改 MQTT 服务器地址和端口（如果使用了 MQTT）。
3.  **编译下载**：使用 Keil 打开 `Projek.uvprojx`，编译无误后通过 ST-Link 下载到单片机。
4.  **运行**：上电后，OLED 应显示初始化界面。等待 ESP8266 连接网络成功后，即可在 APP 端查看数据或控制舵机。

## 感谢阅读
1.  **联系方式 (Contact)**：
    * 如果您在使用或理解本项目代码时遇到任何问题，欢迎通过邮件联系我
    * Email: zheegbm@163.com
2.  **贡献与反馈 (Contribution)**：
    * 欢迎提交 Issue 或 Pull Request 来改进代码！
    * 下面是目录结构

## 目录结构 (Directory Structure)

```text
32温湿度6.0/
├── User/           # main.c, 中断服务函数
├── Hardware/       # 硬件驱动 (DHT11, OLED, PWM, LED, KEY)
├── ESP/            # ESP8266 WiFi 驱动与通信逻辑
├── System/         # 延时函数, 系统配置
├── Library/        # STM32 标准外设库文件
├── Start/          # 启动文件与内核文件
└── ...
