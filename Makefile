SRCS = $(wildcard *.c)
CFLAGS = -Wall -O2
PROGS = $(patsubst %.c, %, $(SRCS))

all: $(PROGS)

clean:
	rm -f $(PROGS)
