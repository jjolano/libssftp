include $(PSL1GHT)/ppu_rules

TARGET = lib$(LIBNAME).a

MACH := $(shell $(CC) -dumpmachine)

CFILES = $(wildcard util/*.c) $(wildcard server/*.c) $(wildcard commands/*.c) compat/fs/$(TYPE).c
OFILES = $(CFILES:.c=.$(MACH).o)

INCLUDE = -I. -I$(PORTLIBS)/include $(LIBPSL1GHT_INC)
CFLAGS = -O2 -Wall -ffunction-sections -fdata-sections -mregnames -mcpu=cell $(MACHDEP)
CFLAGS += -D__PSL1GHT__ -D_FILE_OFFSET_BITS=64

all: $(TARGET)

clean:
	rm -f $(TARGET) $(OFILES)

%.$(MACH).o: %.c
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OFILES)
	$(AR) rcs $@ $^
