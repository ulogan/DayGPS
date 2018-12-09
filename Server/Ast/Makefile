# Astrolog (Version 5.05) File: Makefile (Unix version)
#
# IMPORTANT NOTICE: The graphics database and chart display routines
# used in this program are Copyright (C) 1991-1995 by Walter D. Pullen
# (astara@u.washington.edu). Permission is granted to freely use and
# distribute these routines provided one doesn't sell, restrict, or
# profit from them in any way. Modification is allowed provided these
# notices remain with any altered or edited versions of the program.
#
# First created 11/21/1991.
#
# This Makefile is included only for convenience. One could easily compile
# Astrolog on a Unix system by hand with the command:
# % cc -c -O *.c; cc -o astrolog *.o -lm -lX11
# Generally, all that needs to be done to compile once astrolog.h has been
# edited, is compile each source file, and link them together with the math
# library, and if applicable, the main X library.
#
NAME = astrolog
OBJ = data.o data2.o general.o io.o desa.o\
 calc.o matrix.o placalc.o placalc2.o\
 charts0.o charts1.o charts2.o charts3.o intrpret.o
# xdata.o xgeneral.o xdevice.o xcharts0.o xcharts1.o xcharts2.o xscreen.o
# If you don't have X windows, delete the "-lX11" part from the line below:
#LIBS = -lm -lX11
LIBS = -lm
CFLAGS = -g -w

all : libastrolog.a

aall : astrolog libastrolog.a desa

astrolog: astrolog.o $(OBJ)
	gcc $(CFLAGS) -o $(NAME) astrolog.o $(OBJ) $(LIBS)
#	strip $(NAME)

libastrolog.a: astrologmain.o $(OBJ)
	ar -crs libastrolog.a astrologmain.o $(OBJ)

desa:
	gcc $(CFLAGS) -DDESA_MAIN -o $@ $@.c

astrologmain.o: astrologmain.c
	gcc $(CFLAGS) -c -DTRANSIT astrologmain.c

.c.o:
	gcc $(CFLAGS) -c $?

clean:
	rm -f *.o desa $(NAME) 
