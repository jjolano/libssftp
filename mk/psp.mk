TYPE ?= psp
LIBNAME ?= ssftp_$(TYPE)

TARGET_LIB = lib$(LIBNAME).a

MACH := $(shell $(CC) -dumpmachine)

SRCS = $(wildcard util/*.c) $(wildcard server/*.c) $(wildcard commands/*.c) compat/fs/posix.c
OBJS = $(SRCS:.c=.$(MACH).o)

INCDIR = .
CFLAGS = -O2 -Wall -ffunction-sections -fdata-sections
CFLAGS += -D__USE_SELECT__ -D_FILE_OFFSET_BITS=64

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
