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

all: libaio echo_client_aio echo_client_sync

libaio: libaio.o $(DEPS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

echo_client_aio: echo_client_aio.o $(DEPS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

echo_client_sync: echo_client_sync.o $(DEPS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
