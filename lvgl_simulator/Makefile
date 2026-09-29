# LVGL PC Simulator Makefile

CC = gcc
CFLAGS = -O2 -g -Wall -Wextra -DLVGL_SIMULATOR
CFLAGS += -I$(LVGL_DIR) -I$(LVGL_DIR)/lvgl -I$(LVGL_DIR)/lvgl/src
CFLAGS += $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs) -lpthread

LVGL_DIR = ../UI

# 只编译需要的 LVGL 核心文件
LVGL_SRCS = $(LVGL_DIR)/lvgl/src/lv_core/lv_disp.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_core/lv_group.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_core/lv_indev.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_core/lv_obj.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_core/lv_refr.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_core/lv_style.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_draw/lv_draw_arc.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_draw/lv_draw_blend.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_draw/lv_draw_img.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_draw/lv_draw_label.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_draw/lv_draw_line.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_draw/lv_draw_mask.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_draw/lv_draw_rect.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_draw/lv_draw_triangle.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_draw/lv_img_buf.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_draw/lv_img_cache.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_draw/lv_img_decoder.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_font/lv_font.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_font/lv_font_fmt_txt.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_font/lv_font_loader.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_hal/lv_hal_disp.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_hal/lv_hal_indev.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_hal/lv_hal_tick.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_misc/lv_anim.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_misc/lv_area.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_misc/lv_async.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_misc/lv_bidi.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_misc/lv_color.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_misc/lv_debug.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_misc/lv_gc.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_misc/lv_ll.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_misc/lv_log.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_misc/lv_math.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_misc/lv_mem.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_misc/lv_printf.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_misc/lv_task.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_misc/lv_templ.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_misc/lv_txt.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_misc/lv_txt_ap.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_misc/lv_utils.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_misc/lv_fs.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_themes/lv_theme.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_themes/lv_theme_empty.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_themes/lv_theme_material.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_themes/lv_theme_mono.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_themes/lv_theme_template.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_btn.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_btnmatrix.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_bar.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_calendar.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_canvas.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_chart.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_checkbox.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_cont.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_cpicker.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_dropdown.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_gauge.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_img.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_imgbtn.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_keyboard.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_label.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_led.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_line.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_list.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_linemeter.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_msgbox.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_objmask.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_objx_templ.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_page.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_roller.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_slider.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_spinbox.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_spinner.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_switch.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_table.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_tabview.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_textarea.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_tileview.c
LVGL_SRCS += $(LVGL_DIR)/lvgl/src/lv_widgets/lv_win.c

# 自定义字体
LVGL_SRCS += $(LVGL_DIR)/resources/fonts/jetbrains_mono_bold_20.c
LVGL_SRCS += $(LVGL_DIR)/resources/fonts/jetbrains_mono_extrabold_compressed.c
LVGL_SRCS += $(LVGL_DIR)/resources/fonts/jetbrains_mono_42.c
LVGL_SRCS += $(LVGL_DIR)/resources/fonts/jetbrains_mono_76.c
LVGL_SRCS += $(LVGL_DIR)/resources/fonts/open_sans_light.c
LVGL_SRCS += $(LVGL_DIR)/resources/fonts/fontawesome_weathericons.c
LVGL_SRCS += $(LVGL_DIR)/resources/fonts/fontawesome_icons.c

# 项目源文件
PROJECT_SRCS = $(LVGL_DIR)/infinitime_adapter.c
PROJECT_SRCS += $(LVGL_DIR)/infinitime_bridge.c

# 模拟器源文件
SIM_SRCS = main.c

# 所有源文件
SRCS = $(LVGL_SRCS) $(PROJECT_SRCS) $(SIM_SRCS)
OBJS = $(SRCS:.c=.o)

# 目标
TARGET = lvgl_simulator

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run