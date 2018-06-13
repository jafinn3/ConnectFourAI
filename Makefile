CC=gcc
CFLAGS=-Wall -Wextra -Werror -Wshadow -std=c99 -pedantic -g -fwrapv
FILES=four-network.c four-population.c four-subnet.c lib/utils.c lib/four-board.c connect-four.c
.PHONY: c4 c4d clean
default: c4 c4d 

c4: $(FILES)
	$(CC) -o c4 $(CFLAGS) $(FILES) -lm

c4d: $(FILES)
	$(CC) -DDEBUG -o c4d $(CFLAGS) $(FILES) -lm

clean:
	rm -Rf c4 c4d
