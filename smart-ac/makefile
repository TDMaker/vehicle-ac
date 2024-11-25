CC = cc
LD = cc
SRCS = $(wildcard *.c)
OBJS = $(patsubst %c, %o, $(SRCS))
INCLUDE = -I.
LIB = -lgmp -lpbc
CFLAGS = -Wall -O3

TARGET = a.out

.PHONY:all clean

all: $(TARGET)
$(TARGET): $(OBJS)
	$(LD) -o $@ $^ $(LIB)

%.o:%.c
	$(CC) -c $^ $(INCLUDE) $(CFLAGS) 

clean:
	rm -f $(OBJS) $(TARGET)
