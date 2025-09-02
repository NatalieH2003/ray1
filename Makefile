# Minimal, clear Makefile per the class notes
CC=gcc
FLAGS=-g -Wall -std=gnu99
LIBS=-lm

TARGET=ray1
SRC=ray1.c
HDR=stb_image_write.h

$(TARGET): $(SRC) $(HDR)
	$(CC) $(FLAGS) -o $(TARGET) $(SRC) $(LIBS)

.PHONY: run clean
run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) checkerboard.png gradient.png gradient-dither.png
	rm -f output/checkerboard.png output/gradient.png output/gradient-dither.png
