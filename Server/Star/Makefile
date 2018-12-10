#
ALIBS = ../Ast/libastrolog.a
LIBS = -lm
CFLAGS = -g

all: star

star: star.o $(ALIBS)
	gcc $(CFLAGS) -o $@ $@.o $(ALIBS) $(LIBS)

.c.o:
	gcc $(CFLAGS) -c $?

clean:
	rm -f *.o star

install:
	cp star /var/www/cgi-bin/rasi
