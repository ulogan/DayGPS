#
ALIBS = ../Ast/libastrolog.a
LIBS = -lm
CFLAGS = -g

all: rasi

rasi: rasi.o $(ALIBS)
	gc $(CFLAGS) -o $@ $@.o $(ALIBS) $(LIBS)

.c.o:
	gcc $(CFLAGS) -c $?

clean:
	rm -f *.o rasi

install:
	cp rasi /var/www/cgi-bin/rasi
	cp rasi.html /var/www/html/rasi.html
