CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -I../../include -g
LIB_PATH = ../../lib/librift0.a

all: csv_parser

csv_parser: csv_parser.c $(LIB_PATH)
	$(CC) $(CFLAGS) $< $(LIB_PATH) -lm -o $@

test: csv_parser
	./csv_parser

clean:
	rm -f csv_parser

$(LIB_PATH):
	cd ../.. && make lib
