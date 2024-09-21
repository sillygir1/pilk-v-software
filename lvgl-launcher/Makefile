#
# Makefile
#
CC 				= ~/coding/duo-buildroot-sdk/host-tools/gcc/riscv64-linux-musl-x86_64/bin/riscv64-unknown-linux-musl-gcc
LVGL_DIR_NAME 	?= lvgl
LVGL_DIR 		?= ../lvgl
ENC_DIR = ../duo-encoder-kb
ADC_DIR = ../duo-battery
VM_DIR = ../view-manager

WARNINGS		:= -Wall -Wshadow -Wundef -Wmissing-prototypes -Wno-discarded-qualifiers -Wall -Wextra -Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith \
					-fno-strict-aliasing -Wno-error=cpp -Wuninitialized -Wmaybe-uninitialized -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess \
					-Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic \
					-Wno-sign-compare -Wno-error=missing-prototypes -Wdouble-promotion -Wclobbered -Wdeprecated -Wempty-body -Wtype-limits -Wshift-negative-value -Wstack-usage=2048 \
					-Wno-unused-value -Wno-unused-parameter -Wno-missing-field-initializers -Wuninitialized -Wmaybe-uninitialized -Wall -Wextra -Wno-unused-parameter \
					-Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wpointer-arith -Wno-cast-qual -Wmissing-prototypes \
					-Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wno-discarded-qualifiers -Wformat-security -Wno-ignored-qualifiers -Wno-sign-compare -std=c99
CFLAGS 			?= -O3 -g0 -mcpu=c906fdv -march=rv64imafdcv0p7xthead -mcmodel=medany -mabi=lp64d -Wall -Wshadow 
CFLAGS 			+= -I$(LVGL_DIR)/ -I$(ENC_DIR)/ -I$(ADC_DIR)/ -I$(VM_DIR)/ $(WARNINGS) -DPLUGIN
LDFLAGS 		?= -lm
BIN 			= launcher
BUILD_DIR 		= ./build
BUILD_OBJ_DIR 	= $(BUILD_DIR)/obj
BUILD_BIN_DIR 	= $(BUILD_DIR)/bin

prefix 			?= /usr
bindir 			?= $(prefix)/bin

#Collect the files to compile
MAINSRC = 		./src/launcher.c

include $(LVGL_DIR)/lvgl/lvgl.mk
include $(LVGL_DIR)/lv_drivers/lv_drivers.mk

CSRCS 			+=$(ENC_DIR)/encoder.c 
CSRCS 			+=$(ADC_DIR)/battery.c 
CSRCS 			+=$(VM_DIR)/view_manager.c 
CSRCS 			+=$(VM_DIR)/view_manager_extra.c 
CSRCS 			+=./src/views/apps_screen_view.c
CSRCS 			+=./src/views/main_screen_view.c
CSRCS 			+=./src/views/view_list.c

OBJEXT 			?= .o

AOBJS 			= $(ASRCS:.S=$(OBJEXT))
COBJS 			= $(CSRCS:.c=$(OBJEXT))

MAINOBJ 		= $(MAINSRC:.c=$(OBJEXT))

SRCS 			= $(ASRCS) $(MAINSRC)
OBJS 			= $(AOBJS) $(COBJS) $(MAINOBJ)
TARGET 			= $(addprefix $(BUILD_OBJ_DIR)/, $(patsubst ./%, %, $(OBJS)))

## MAINOBJ -> OBJFILES

all: default

$(BUILD_OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC)  $(CFLAGS) -c $< -o $@
	@echo "CC $<"
    
default: $(TARGET)
	@mkdir -p $(dir $(BUILD_BIN_DIR)/)
	$(CC) -o $(BUILD_BIN_DIR)/$(BIN) $(TARGET) $(CFLAGS) $(LDFLAGS)

clean: 
	rm -rf $(BUILD_DIR)/bin
	rm -rf $(BUILD_DIR)/duo-battery
	rm -rf $(BUILD_DIR)/duo-encoder-kb
	rm -rf $(BUILD_DIR)/view-manager
	@if [ -d "$(BUILD_DIR)/obj/" ]; then\
		cd $(BUILD_DIR)/obj/ && find . -type f ! -name 'lv_*.o' -delete;\
	fi

clean-all: 
	rm -rf $(BUILD_DIR)

install:
	install -d $(DESTDIR)$(bindir)
	install $(BUILD_BIN_DIR)/$(BIN) $(DESTDIR)$(bindir)

uninstall:
	$(RM) -r $(addprefix $(DESTDIR)$(bindir)/,$(BIN))
