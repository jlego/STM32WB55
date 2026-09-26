# InfiniTime UI 移植到 STM32WB55

## 移植概述

本项目已成功将 InfiniTime-stm32 的完整 UI 系统移植到 STM32WB55 项目。

### 已移植的组件

#### 1. LVGL 图形库 (277 个文件)
- LVGL v7.7.0 核心库
- 配置文件 (lv_conf.h)
- 显示驱动适配层
- 触摸驱动适配层

#### 2. DisplayApp 应用框架 (294 个文件)
- 屏幕管理系统
- 所有表盘界面 (WatchFace)
- 设置屏幕
- 通知屏幕
- 应用程序列表
- 图标和字体资源

#### 3. 控制器组件 (67 个文件)
- 电池控制器
- 亮度控制器
- 日期时间控制器
- 设置控制器
- 运动控制器
- 心率控制器
- 马达控制器
- 闹钟控制器
- 秒表控制器
- 定时器控制器

#### 4. 驱动程序 (47 个文件)
- ST7789 显示驱动
- CST816S 触摸驱动
- SPI 驱动
- I2C 驱动
- Flash 驱动

#### 5. 触摸和按钮处理 (5 个文件)
- TouchHandler 触摸手势识别
- ButtonHandler 按钮处理

#### 6. 工具库 (6 个文件)
- 静态栈
- 环形缓冲区
- 其他辅助工具

#### 7. 资源文件 (16 个文件)
- 字体资源
- 图像资源

#### 8. 适配层 (14 个文件)
- LVGL STM32 适配
- InfiniTime 适配层
- 触摸驱动适配

## 项目结构

```
STM32WB55/
├── Core/
│   ├── Inc/
│   │   └── st7789.h          # ST7789 显示驱动头文件
│   └── Src/
│       └── main.c            # 主程序（已集成 UI）
└── UI/
    ├── lvgl/                  # LVGL 图形库
    ├── lv_conf.h             # LVGL 配置文件
    ├── lvgl_stm32.h/c        # LVGL STM32 适配层
    ├── displayapp/           # InfiniTime 显示应用
    │   ├── screens/          # 所有屏幕
    │   ├── apps/             # 应用程序
    │   ├── fonts/            # 字体资源
    │   ├── icons/            # 图标资源
    │   └── widgets/          # 控件
    ├── components/           # 控制器组件
    ├── drivers/              # 驱动程序
    ├── touchhandler/         # 触摸处理
    ├── buttonhandler/        # 按钮处理
    ├── utility/              # 工具库
    ├── resources/            # 资源文件
    ├── touch_driver.h/c      # 触摸驱动适配
    ├── infinitime_adapter.h/c # InfiniTime 适配层
    └── ui_demo.h/c           # UI 演示程序
```

## 使用方法

### 1. 初始化 UI 系统

在 main.c 中已经集成：

```c
#include "infinitime_adapter.h"

// 在初始化部分
ST7789_Init();
infinitime_ui_init();
```

### 2. 在主循环中处理 UI

```c
while (1) {
    uint32_t current_tick = HAL_GetTick();
    uint32_t tick_diff = current_tick - last_tick;
    
    if (tick_diff >= 5) {
        lv_tick_inc(tick_diff);
        last_tick = current_tick;
    }
    
    infinitime_ui_task();
}
```

### 3. 触摸支持

触摸驱动已集成，但需要配置 I2C：

1. 在 STM32CubeMX 中启用 I2C1
2. 定义 `USE_HARDWARE_I2C` 宏
3. 配置触摸引脚

## 当前状态

### ✅ 已完成
- [x] LVGL 库移植
- [x] 显示驱动适配
- [x] 触摸驱动框架
- [x] UI 初始化集成
- [x] 主循环集成
- [x] 所有屏幕文件复制
- [x] 所有组件文件复制
- [x] 字体和图标资源

### ⚠️ 需要后续工作
- [ ] I2C 硬件配置（用于触摸）
- [ ] C++ 到 C 的适配（InfiniTime 使用 C++）
- [ ] FreeRTOS 任务适配（或使用轮询模式）
- [ ] 文件系统支持（LittleFS）
- [ ] BLE 功能集成
- [ ] 具体屏幕功能的启用

## 编译说明

### 需要添加的源文件到项目

在 STM32CubeIDE 中，需要将以下文件添加到项目：

1. **LVGL 库**: `UI/lvgl/src/**/*.c`
2. **UI 适配层**: 
   - `UI/lvgl_stm32.c`
   - `UI/infinitime_adapter.c`
   - `UI/touch_driver.c`
3. **显示驱动**: `Core/Src/st7789.c`

### 包含路径

需要添加以下包含路径：
- `UI/`
- `UI/lvgl/`
- `UI/lvgl/src/`
- `Core/Inc/`

### 宏定义

- `LV_CONF_INCLUDE_SIMPLE` - 使用简单的 LVGL 配置包含
- `USE_HARDWARE_I2C` - 如果使用硬件 I2C 触摸

## 注意事项

1. **内存使用**: LVGL 需要较多 RAM，确保 STM32WB55 有足够的内存
2. **性能**: SPI 显示刷新速度可能影响 UI 流畅度
3. **触摸**: 需要正确配置 I2C 才能使用触摸功能
4. **C++ 代码**: InfiniTime 原始代码使用 C++，本项目使用 C，需要适配

## 下一步建议

1. 配置 I2C 用于触摸支持
2. 实现具体的屏幕切换逻辑
3. 添加按钮支持（如果需要）
4. 集成 BLE 功能
5. 添加文件系统支持（用于保存设置）

## 参考资源

- [LVGL 文档](https://docs.lvgl.io/)
- [InfiniTime 原始项目](https://github.com/JF002/InfiniTime)
- [STM32WB55 参考手册](https://www.st.com/en/microcontrollers-microprocessors/stm32wb55.html)