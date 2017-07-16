TYPE ?= vita
LIBNAME ?= ssftp_$(TYPE)

PREFIX := $(VITASDK)/bin/arm-vita-eabi-

CC := $(PREFIX)gcc
AR := $(PREFIX)ar
LD := $(PREFIX)ld

TARGET = lib$(LIBNAME).a

MACH := $(shell $(CC) -dumpmachine)

CFILES = $(wildcard util/*.c) $(wildcard server/*.c) $(wildcard commands/*.c) compat/fs/$(TYPE).c
OFILES = $(CFILES:.c=.$(MACH).o)

INCLUDE = -I. $(VITASDK)/include
CFLAGS = -O2 -Wall -ffunction-sections -fdata-sections
CFLAGS += -D__USE_SELECT__ -D__VITA__ -D_FILE_OFFSET_BITS=64

all: $(TARGET)

clean:
	rm -f $(TARGET) $(OFILES)

%.$(MACH).o: %.c
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OFILES)
	$(AR) rcs $@ $^
