TARGET = lab1
CC = g++
CFLAGS = -g -std=c++11

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = main.o
HEADERS =

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c$<-o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)