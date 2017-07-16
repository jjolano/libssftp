TYPE ?= psp
LIBNAME ?= ssftp_$(TYPE)

TARGET_LIB = lib$(LIBNAME).a

SRCS = $(wildcard util/*.c) $(wildcard server/*.c) $(wildcard commands/*.c) compat/fs/$(TYPE).c compat/selectpoll.c
OBJS = $(SRCS:.c=.o)

INCDIR = .
CFLAGS = -O2 -Wall -ffunction-sections -fdata-sections
CFLAGS += -D__USE_SELECT__ -D_FILE_OFFSET_BITS=64

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
