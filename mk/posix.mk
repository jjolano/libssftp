TARGET = lib$(LIBNAME).a

MACH = $(shell uname -m)

CFILES = $(wildcard util/*.c) $(wildcard server/*.c) $(wildcard commands/*.c)
OFILES = $(CFILES:.c=.$(MACH).o)

LIBS = -l$(LIBNAME)
LIBPATH = -L.
INCLUDE = -I.
CFLAGS = -Og -g -Wall -ffunction-sections -fdata-sections -fno-builtin-printf

all: $(TARGET)

test.elf: all
	$(CC) $(INCLUDE) $(LIBPATH) $(LIBS) $(CFLAGS) -o $@ test/main.c

clean:
	rm -f $(TARGET) $(OFILES) test.elf

%.$(MACH).o: %.c
	$(CC) $(INCLUDE) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OFILES)
	$(AR) rcs $@ $^
