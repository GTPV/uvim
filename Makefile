PROGRAM=uvim

SOURCE=main.c

OBJECTS=$(SOURCE:.c=.o)

CC=gcc
WARNINGS=-Wall -Wextra -Werror
CFLAGS=-std=c99 -O3 $(WARNINGS)

$(PROGRAM): $(SOURCE)
	$(CC) $(CFLAGS) -o $(PROGRAM) $(SOURCE)

clean:
	rm -f $(PROGRAM) $(OBJECTS)
