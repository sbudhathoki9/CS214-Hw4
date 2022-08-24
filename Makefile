CC = gcc
CFLAGS = -Wall -Wvla -g -fsanitize=address
LDFLAGS = 
OBJFILES = mymalloc.o memperf.o
TARGET = memperf

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET) *~