CC = gcc
CFLAGS = -Wall -g -fPIC -Wextra
LDFLAGS = -shared

SRC = malloc.c flist.c
OBJ = $(SRC:.c=.o)
SO_TARGET = malloc.so

all: $(SO_TARGET)

$(SO_TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test:
	$(CC) $(CFLAGS) malloc_test.c -o mtest

clean:
	rm -f $(OBJ) $(SO_TARGET) mtest

