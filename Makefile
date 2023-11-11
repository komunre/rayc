CC=g++
CFLAGS=-g

all:
	$(CC) $(CFLAGS) -o rend main.c tgaimage.cpp

clean:
	rm -f *.tga