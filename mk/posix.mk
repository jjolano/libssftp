TARGET = $(LIBNAME).a

MACH = $(shell uname -m)

CFILES = $(wildcard util/*.c) $(wildcard server/*.c)
OFILES = $(CFILES:.c=.$(MACH).o)

INCLUDE = -I.
CFLAGS = -Og -g -Wall -ffunction-sections -fdata-sections -fno-builtin-printf

all: $(TARGET)

clean:
	rm -f $(TARGET) $(OFILES)

%.$(MACH).o: %.c
	$(CC) $(INCLUDE) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OFILES)
	$(AR) rcs $@ $^
