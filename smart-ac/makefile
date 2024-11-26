CC = cc
LD = cc
SRCS = $(wildcard src/*.c)
OBJS = $(patsubst %c, %o, $(SRCS))
info:
	@echo "SRCS: $(SRCS)"
	@echo "OBJS: $(OBJS)"
INCLUDE = -I./include
LIB = -lgmp -lpbc
CFLAGS = -Wall -O3

TARGET = a.out

.PHONY:all clean

all: $(TARGET)
$(TARGET): $(OBJS)
	$(LD) -o $@ $^ $(LIB)

src/%.o:src/%.c
	$(CC) -c $< -o $@ $(INCLUDE) $(CFLAGS)
#$(CC) -c $^ $(INCLUDE) $(CFLAGS) 

clean:
	rm -f $(OBJS) $(TARGET)
