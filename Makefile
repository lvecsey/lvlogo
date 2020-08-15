
CC=gcc

CFLAGS=-O3 -Wall -g -pg

LIBS=-lm

/tmp/output_lvlogo.png : RENDER_RES=500x500

/tmp/output_lvlogo.png : lvlogo
	@./$^ $(RENDER_RES) | convert -size $(RENDER_RES) rgb:- -depth 32 $@

lvlogo : writefile.o lvlogo.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(LIBS)
