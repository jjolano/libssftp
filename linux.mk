TARGET = $(LIBNAME).a

CFILES = $(wildcard util/*.c) $(wildcard server/*.c)
OFILES = $(CFILES:.c=.o)

INCLUDE = -I.
CFLAGS = -Og -g -Wall -ffunction-sections -fdata-sections -fno-builtin-printf

all: $(TARGET)

clean:
	rm -f $(TARGET) $(OFILES)

%.o: %.c
	$(CC) $(INCLUDE) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OFILES)
	$(AR) rcs $@ $^
