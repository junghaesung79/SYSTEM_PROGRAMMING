CC = clang
CFLAGS = -Wall -I./include
TARGET = myshell

SRCS = $(wildcard src/*.c) $(wildcard src/commands/*.c)
OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
