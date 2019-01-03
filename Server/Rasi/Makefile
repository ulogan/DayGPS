#
ALIBS = ../Ast/libastrolog.a
LIBS = -lm
CFLAGS = -g

all: rasi rasiClient

rasiClient: rasiClient.o
	gcc $(CFLAGS) -o $@ $@.o 

rasi: rasi.o $(ALIBS)
	gcc $(CFLAGS) -o $@ $@.o $(ALIBS) $(LIBS)

.c.o:
	gcc $(CFLAGS) -c $?

clean:
	rm -f *.o rasi rasiClient

install:
	cp rasi /var/www/cgi-bin/rasi
	cp rasi.html /var/www/html/rasi.html
