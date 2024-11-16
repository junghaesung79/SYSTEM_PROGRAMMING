CC = clang
CFLAGS = -Wall -I./include
TARGET = myshell
SRCS = $(wildcard src/*.c) $(wildcard src/commands/*.c)
OBJS = $(SRCS:.c=.o)

# 테스트 프로그램 관련 변수
TEST_SRCS = $(wildcard test/*.c)
TEST_BINS = $(TEST_SRCS:.c=)

all: $(TARGET) test_programs install

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 테스트 프로그램 컴파일
test_programs: $(TEST_BINS)

test/%: test/%.c
	$(CC) $(CFLAGS) $< -o $@

# ROOT_PATH 생성 및 테스트 프로그램 설치
install: test_programs
	mkdir -p /tmp/test
	cp $(TEST_BINS) /tmp/test/

clean:
	rm -f $(TARGET) $(OBJS) $(TEST_BINS)
	rm -f /tmp/test/infinite /tmp/test/count

.PHONY: all clean test_programs install
