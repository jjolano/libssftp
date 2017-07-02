TARGET = lib$(LIBNAME).a

MACH := $(shell $(CC) -dumpmachine)

CFILES = $(wildcard util/*.c) $(wildcard server/*.c) $(wildcard commands/*.c) compat/fs/$(TYPE).c
OFILES = $(CFILES:.c=.$(MACH).o)

LIBS = -l$(LIBNAME)
LIBPATH = -L.
INCLUDE = -I.
CFLAGS = -Og -g -Wall -ffunction-sections -fdata-sections
CFLAGS += -D_FILE_OFFSET_BITS=64

all: $(TARGET)

test.elf: all
	$(CC) $(INCLUDE) $(LIBPATH) $(CFLAGS) -o $@ test/main.c $(LIBS)

clean:
	rm -f $(TARGET) $(OFILES) test.elf

%.$(MACH).o: %.c
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OFILES)
	$(AR) rcs $@ $^
