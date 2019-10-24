IDIR =../include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=obj
LDIR =../lib

LIBS=-laio

_DEPS = 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))



$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: libaio socket_client_aio

libaio: libaio.o $(DEPS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

socket_client_aio: socket_aio.o $(DEPS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
