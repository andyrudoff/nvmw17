#
# helloworld Makefile
#
PROGS = hellowrite helloread
CFLAGS = -ggdb -Wall -Werror
LIBS = -lpmemobj -lpmem -pthread

all: $(PROGS)

hellowrite: hellowrite.o layout.h
	$(CC) -o $@ $^ $(LIBS)

helloread: helloread.o layout.h
	$(CC) -o $@ $^ $(LIBS)

clean:
	$(RM) *.o a.out core

clobber: clean
	$(RM) $(PROGS)

.PHONY: clobber
