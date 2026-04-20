CC = gcc
CFLAGS = -O2 -Wall -Wextra -std=c11 -fPIC
SRC = minesweeper.c queue.c

ifeq ($(OS),Windows_NT)
TARGET = minesweeper.dll
LDFLAGS = -shared
else
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
TARGET = libminesweeper.dylib
LDFLAGS = -dynamiclib
else
TARGET = libminesweeper.so
LDFLAGS = -shared
endif
endif

all: $(TARGET)

$(TARGET): $(SRC) minesweeper.h queue.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC)

clean:
	-del /Q *.o *.so *.dylib *.dll 2>nul || true
