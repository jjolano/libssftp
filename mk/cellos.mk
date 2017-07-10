TYPE ?= cellos
LIBNAME ?= ssftp_$(TYPE)

CELL_SDK ?= /usr/local/cell
CELL_MK_DIR ?= $(CELL_SDK)/samples/mk

include $(CELL_MK_DIR)/sdk.makedef.mk

OBJS_DIR = $(TYPE)_objs

PPU_LIB_TARGET = lib$(LIBNAME).a
PPU_OPTIMIZE_LV = -O2

PPU_CFLAGS += -Wall -ffunction-sections -fdata-sections
PPU_SRCS := $(wildcard util/*.c) $(wildcard server/*.c) $(wildcard commands/*.c) compat/fs/$(TYPE).c compat/libftp.c
PPU_INCDIRS += -I.

include $(CELL_MK_DIR)/sdk.target.mk
