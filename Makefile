CC = clang
CFLAGS = -Wall -I./include
TARGET = myshell
SRCS = $(wildcard src/*.c) $(wildcard src/commands/*.c)
OBJS = $(SRCS:.c=.o)

all: $(TARGET) run

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)