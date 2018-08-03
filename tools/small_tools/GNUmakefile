# Stand-alone build rules for utilities in this directory:

CFLAGS          := --std=c99 -W -Wall -Wextra

CSRCS           := ${wildcard *.c}
PROGS           := ${CSRCS:%.c=%}

Q               := @
I               := -
C               := $Q $I
R               := /bin/rm -f

all:                    ${PROGS}
clean:          ; $C $R ${PROGS}
