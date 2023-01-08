CC=g++
CFLAGS= -lpthread
DEPS=tema1.h
OBJ=tema1.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

build: $(OBJ)
	$(CC) -o tema1 $^ $(CFLAGS)
	rm -f *.o

.PHONY: clean

clean:
	rm -f tema1