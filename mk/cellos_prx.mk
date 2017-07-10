TYPE ?= cellos_prx
LIBNAME ?= ssftp_$(TYPE)

CELL_SDK ?= /usr/local/cell
CELL_MK_DIR ?= $(CELL_SDK)/samples/mk

include $(CELL_MK_DIR)/sdk.makedef.mk

OBJS_DIR = $(TYPE)_objs

PPU_LIB_TARGET = lib$(LIBNAME).a
PPU_OPTIMIZE_LV = -Os

PPU_CFLAGS += -Wall -ffunction-sections -fdata-sections --param max-inline-insns-single=1000
PPU_SRCS := $(wildcard util/*.c) $(wildcard server/*.c) $(wildcard commands/*.c) compat/fs/cellos.c compat/libftp.c
PPU_INCDIRS += -I.

PPU_CFLAGS += -D__CELLOS_PRX__

include $(CELL_MK_DIR)/sdk.target.mk
