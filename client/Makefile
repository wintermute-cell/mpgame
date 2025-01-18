CC = gcc
CFLAGS = -Wall -Wextra
INCLUDES = -I./vendor/raylib/include/
LDFLAGS = -L./vendor/raylib/lib/ -lm -pthread -ldl -l:libraylib.a

SRCS = main.c
TARGET = client

$(TARGET): $(SRCS)
	bear -- $(CC) $(SRCS) -o $(TARGET) $(LDFLAGS) $(INCLUDES)

.PHONY: clean
clean:
	rm -f $(TARGET)

.PHONY: dev
dev: $(TARGET)
	./$(TARGET)

