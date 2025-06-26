# Makefile for rift_common shared library
# Aegis Development Team

CC=gcc
CFLAGS=-Wall -Wextra -Werror -fPIC -pthread -O2 -g
LDFLAGS=-shared -pthread

SRCDIR=.
BUILDDIR=../../build
LIBDIR=$(BUILDDIR)/lib
TARGET=$(LIBDIR)/librift_common.so

SOURCES=rift_common.c rift_telemetry.c rift_governance.c
OBJECTS=$(SOURCES:.c=.o)
HEADERS=rift_common.h rift_telemetry.h rift_governance.h

.PHONY: all clean install

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(LIBDIR)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBDIR):
	mkdir -p $(LIBDIR)

clean:
	rm -f *.o $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/local/lib/
	cp $(HEADERS) /usr/local/include/

test: $(TARGET)
	gcc -o test_common test_common.c -L$(LIBDIR) -lrift_common -pthread
