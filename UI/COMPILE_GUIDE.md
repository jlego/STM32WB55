# UI移植编译指南

## 需要添加到项目的源文件

### 1. LVGL核心库 (111个文件)
```
UI/lvgl/src/lv_core/lv_obj.c
UI/lvgl/src/lv_core/lv_group.c
UI/lvgl/src/lv_core/lv_indev.c
UI/lvgl/src/lv_core/lv_refr.c
UI/lvgl/src/lv_core/lv_style.c
UI/lvgl/src/lv_core/lv_debug.c
UI/lvgl/src/lv_draw/lv_draw_arc.c
UI/lvgl/src/lv_draw/lv_draw_blend.c
UI/lvgl/src/lv_draw/lv_draw_img.c
UI/lvgl/src/lv_draw/lv_draw_label.c
UI/lvgl/src/lv_draw/lv_draw_line.c
UI/lvgl/src/lv_draw/lv_draw_mask.c
UI/lvgl/src/lv_draw/lv_draw_rect.c
UI/lvgl/src/lv_draw/lv_draw_triangle.c
UI/lvgl/src/lv_draw/lv_draw_img_decoder.c
UI/lvgl/src/lv_draw/lv_img_cache.c
UI/lvgl/src/lv_draw/lv_img_decoder.c
UI/lvgl/src/lv_font/lv_font.c
UI/lvgl/src/lv_font/lv_font_dejavu_16_persian_hebrew.c
UI/lvgl/src/lv_font/lv_font_fmt_txt.c
UI/lvgl/src/lv_font/lv_font_loader.c
UI/lvgl/src/lv_font/lv_font_loader_v1.c
UI/lvgl/src/lv_font/lv_font_loader_v2.c
UI/lvgl/src/lv_font/lv_font_montserrat_10.c
UI/lvgl/src/lv_font/lv_font_montserrat_12.c
UI/lvgl/src/lv_font/lv_font_montserrat_12_subpx.c
UI/lvgl/src/lv_font/lv_font_montserrat_14.c
UI/lvgl/src/lv_font/lv_font_montserrat_16.c
UI/lvgl/src/lv_font/lv_font_montserrat_18.c
UI/lvgl/src/lv_font/lv_font_montserrat_20.c
UI/lvgl/src/lv_font/lv_font_montserrat_22.c
UI/lvgl/src/lv_font/lv_font_montserrat_24.c
UI/lvgl/src/lv_font/lv_font_montserrat_26.c
UI/lvgl/src/lv_font/lv_font_montserrat_28.c
UI/lvgl/src/lv_font/lv_font_montserrat_28_compressed.c
UI/lvgl/src/lv_font/lv_font_montserrat_30.c
UI/lvgl/src/lv_font/lv_font_montserrat_32.c
UI/lvgl/src/lv_font/lv_font_montserrat_34.c
UI/lvgl/src/lv_font/lv_font_montserrat_36.c
UI/lvgl/src/lv_font/lv_font_montserrat_38.c
UI/lvgl/src/lv_font/lv_font_montserrat_40.c
UI/lvgl/src/lv_font/lv_font_montserrat_42.c
UI/lvgl/src/lv_font/lv_font_montserrat_44.c
UI/lvgl/src/lv_font/lv_font_montserrat_46.c
UI/lvgl/src/lv_font/lv_font_montserrat_48.c
UI/lvgl/src/lv_font/lv_font_montserrat_8.c
UI/lvgl/src/lv_font/lv_font_unscii_16.c
UI/lvgl/src/lv_font/lv_font_unscii_8.c
UI/lvgl/src/lv_gpu/lv_gpu_stm32_dma2d.c
UI/lvgl/src/lv_hal/lv_hal_disp.c
UI/lvgl/src/lv_hal/lv_hal_indev.c
UI/lvgl/src/lv_hal/lv_hal_tick.c
UI/lvgl/src/lv_misc/lv_anim.c
UI/lvgl/src/lv_misc/lv_anim_ll.c
UI/lvgl/src/lv_misc/lv_area.c
UI/lvgl/src/lv_misc/lv_async.c
UI/lvgl/src/lv_misc/lv_bidi.c
UI/lvgl/src/lv_misc/lv_color.c
UI/lvgl/src/lv_misc/lv_debug.c
UI/lvgl/src/lv_misc/lv_fs.c
UI/lvgl/src/lv_misc/lv_gc.c
UI/lvgl/src/lv_misc/lv_ll.c
UI/lvgl/src/lv_misc/lv_log.c
UI/lvgl/src/lv_misc/lv_math.c
UI/lvgl/src/lv_misc/lv_mem.c
UI/lvgl/src/lv_misc/lv_printf.c
UI/lvgl/src/lv_misc/lv_task.c
UI/lvgl/src/lv_misc/lv_templ.c
UI/lvgl/src/lv_misc/lv_txt.c
UI/lvgl/src/lv_misc/lv_txt_ap.c
UI/lvgl/src/lv_misc/lv_utils.c
UI/lvgl/src/lv_themes/lv_theme.c
UI/lvgl/src/lv_themes/lv_theme_alien.c
UI/lvgl/src/lv_themes/lv_theme_default.c
UI/lvgl/src/lv_themes/lv_theme_material.c
UI/lvgl/src/lv_themes/lv_theme_mono.c
UI/lvgl/src/lv_themes/lv_theme_night.c
UI/lvgl/src/lv_themes/lv_theme_template.c
UI/lvgl/src/lv_themes/lv_theme_zen.c
UI/lvgl/src/lv_widgets/lv_arc.c
UI/lvgl/src/lv_widgets/lv_bar.c
UI/lvgl/src/lv_widgets/lv_btn.c
UI/lvgl/src/lv_widgets/lv_btnmatrix.c
UI/lvgl/src/lv_widgets/lv_calendar.c
UI/lvgl/src/lv_widgets/lv_canvas.c
UI/lvgl/src/lv_widgets/lv_chart.c
UI/lvgl/src/lv_widgets/lv_checkbox.c
UI/lvgl/src/lv_widgets/lv_cont.c
UI/lvgl/src/lv_widgets/lv_dropdown.c
UI/lvgl/src/lv_widgets/lv_gauge.c
UI/lvgl/src/lv_widgets/lv_img.c
UI/lvgl/src/lv_widgets/lv_imgbtn.c
UI/lvgl/src/lv_widgets/lv_keyboard.c
UI/lvgl/src/lv_widgets/lv_label.c
UI/lvgl/src/lv_widgets/lv_led.c
UI/lvgl/src/lv_widgets/lv_line.c
UI/lvgl/src/lv_widgets/lv_list.c
UI/lvgl/src/lv_widgets/lv_linemeter.c
UI/lvgl/src/lv_widgets/lv_msgbox.c
UI/lvgl/src/lv_widgets/lv_objmask.c
UI/lvgl/src/lv_widgets/lv_objx_templ.c
UI/lvgl/src/lv_widgets/lv_page.c
UI/lvgl/src/lv_widgets/lv_roller.c
UI/lvgl/src/lv_widgets/lv_slider.c
UI/lvgl/src/lv_widgets/lv_spinbox.c
UI/lvgl/src/lv_widgets/lv_spinner.c
UI/lvgl/src/lv_widgets/lv_switch.c
UI/lvgl/src/lv_widgets/lv_table.c
UI/lvgl/src/lv_widgets/lv_tabview.c
UI/lvgl/src/lv_widgets/lv_textarea.c
UI/lvgl/src/lv_widgets/lv_tileview.c
UI/lvgl/src/lv_widgets/lv_win.c
```

### 2. UI适配层 (3个文件)
```
UI/lvgl_stm32.c
UI/infinitime_adapter.c
UI/soft_i2c.c
UI/ft3168_touch.c
```

### 3. 原有驱动 (1个文件)
```
Core/Src/st7789.c
```

## 头文件包含路径

在IDE中添加以下包含路径：
```
UI/
UI/lvgl/
UI/lvgl/src/
UI/lvgl/src/lv_core/
UI/lvgl/src/lv_draw/
UI/lvgl/src/lv_font/
UI/lvgl/src/lv_hal/
UI/lvgl/src/lv_misc/
UI/lvgl/src/lv_themes/
UI/lvgl/src/lv_widgets/
Core/Inc/
```

## 宏定义

在项目设置中添加以下宏定义：
```
LV_CONF_INCLUDE_SIMPLE
USE_SOFT_I2C
```

## 引脚配置

### FT3168触摸芯片
- **INT**: PA3 (中断输入)
- **RST**: PA2 (复位输出)
- **SDA**: PB7 (I2C数据)
- **SCL**: PB6 (I2C时钟)

### ST7789显示屏
- **CS**: PA15 (片选)
- **DC**: PA0 (数据/命令)
- **RST**: PB0 (复位)
- **SCL**: PA5 (SPI时钟)
- **SDA**: PB5 (SPI数据/MOSI)
- **LED**: PB1 (背光)

## 编译顺序

1. 先编译LVGL核心库
2. 再编译UI适配层
3. 最后编译原有驱动

## 注意事项

1. **内存使用**: LVGL需要较多RAM，建议堆大小至少0x800
2. **栈大小**: 建议栈大小至少0x400
3. **优化级别**: 建议使用-O2优化
4. **触摸测试**: 编译后先测试触摸是否正常工作
5. **显示测试**: 确认LVGL界面能正常显示