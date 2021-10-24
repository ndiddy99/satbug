CC = gcc

TARGET = satbug
CFLAGS = -g -std=gnu99 -Wall

OBJECTS = main.o crc.o devcart.o server.o

all: $(TARGET)

clean:
	rm *.o

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -L/opt/homebrew/lib -lftdi1 -lusb-1.0

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<
